#include "content/nw/src/net/util/embed_utils.h"

#include "base/command_line.h"
#include "base/files/memory_mapped_file.h"
#include "net/base/mime_util.h"
#include "net/base/file_stream_whence.h"
#include "base/files/file_path.h"
#include "third_party/node/deps/uv/include/uv.h"
#include "net/base/gzip_header.h"
#include "third_party/zlib/zlib.h"

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

#ifndef MAGIC_KEY 
#define MAGIC_KEY "\x20\x01\x77\xf3\x66"
#define MAGIC_KEY_END '\x31'
#endif

#ifndef MAGIC_SIZE
#define MAGIC_SIZE 6
#endif

#define CHUNK 16384

using namespace net;
using namespace base;

namespace embed_util {

	FileMetaInfo::FileMetaInfo()
		: file_size(0),
		mime_type_result(true),
		file_exists(true),
		is_directory(false) {
	}
	
	std::map<std::string, embed_util::FileMetaInfo *> *Utility::GetOffsetMap()
	{
		static std::map<std::string, embed_util::FileMetaInfo *> *OffsetMap = new std::map<std::string, embed_util::FileMetaInfo *>();
		return OffsetMap;
	}

	base::FilePath::StringType Utility::GetContainer() {
		static bool cached = false;
		static base::FilePath::StringType _path;
		if(cached) return _path;
		base::FilePath path;
		CommandLine* command_line = CommandLine::ForCurrentProcess();
#if defined(OS_MACOSX)
		path = command_line->GetProgram().DirName().DirName().Append("Resources").Append("Package");
#else
		if(command_line->GetArgs().size() > 0)
			path = base::FilePath(command_line->GetArgs()[0]);
		else {
		  size_t size = 1024;
		  char* execPath = new char[size];
		  if (uv_exepath(execPath, &size) == 0) path = base::FilePath::FromUTF8Unsafe(std::string(execPath, size));
		  else path = base::FilePath(command_line->GetProgram());
		}
#endif
		_path = path.value();
		cached = true;
		return _path;
	}

	int Utility::IndexOf(const unsigned char *data, const size_t length, size_t pos) {
		const unsigned char needle[MAGIC_SIZE] = MAGIC_KEY;
		typedef std::vector<size_t> occtable_type;
		occtable_type occ(UCHAR_MAX+1, MAGIC_SIZE);
    for(int a=0;a<(MAGIC_SIZE-1);a++) occ[needle[a]] = MAGIC_SIZE-(a+1);
    if(MAGIC_SIZE > length - pos) return -1;
		size_t hay_pos=0;
		while((pos + hay_pos) <= length-MAGIC_SIZE) {
			const unsigned char *s = (data + pos + hay_pos);
			if(MAGIC_KEY_END == s[5] && std::memcmp(needle,s,MAGIC_SIZE-1) == 0) return hay_pos+MAGIC_SIZE;
			hay_pos += occ[s[5]];
		}
		return -1;
	}
	
	bool Utility::Load() {
		static bool loaded = false;
		if(loaded==true) return loaded=true;
		std::map<std::string, embed_util::FileMetaInfo *> *OffsetMap = Utility::GetOffsetMap();
		base::MemoryMappedFile mmap_;
		size_t pos=0,a=0,b=0;
		if (!mmap_.Initialize(base::FilePath(Utility::GetContainer()))) {
			DLOG(ERROR) << "Failed to mmap application data (embed_utils.cc:93)";
			return loaded=false;
		}
		const uint8* data = mmap_.data();
		const size_t len = mmap_.length();
		while(pos < len) {
			a=Utility::IndexOf(reinterpret_cast<const unsigned char*>(data),len,pos);
			if( (b=Utility::IndexOf(reinterpret_cast<const unsigned char*>(data),len,pos+a)) < (MAGIC_SIZE+1) ) return loaded=false;
			if(a==b) return loaded=true;
			std::string key = std::string(reinterpret_cast<const char*>(data+pos+a),(b-MAGIC_SIZE));
			(*OffsetMap)[key] = new embed_util::FileMetaInfo();
			(*OffsetMap)[key]->offset = pos+a+b+8;
			(*OffsetMap)[key]->file_size = *reinterpret_cast<const unsigned int *>(data+pos+a+b);
			(*OffsetMap)[key]->mime_type_result = net::GetMimeTypeFromFile(base::FilePath::FromUTF8Unsafe(key), &(*OffsetMap)[key]->mime_type);
			(*OffsetMap)[key]->file_name = key;
			pos = (*OffsetMap)[key]->offset + (*OffsetMap)[key]->file_size;
		}
		return loaded=true;
	}
 
	bool Utility::GetFileInfo(std::string file, embed_util::FileMetaInfo* meta_info) {
		if(!embed_util::Utility::Load()) return false;
		std::map<std::string, embed_util::FileMetaInfo *> *OffsetMap = Utility::GetOffsetMap();
		std::size_t bs = file.find("/",0,1);
		if(bs!=std::string::npos) {
			file = file.replace(0, 1, "");
		}
		if((*OffsetMap).find(file) == (*OffsetMap).end()) return false;
		
		meta_info->file_exists = (*OffsetMap)[file]->file_exists;
		meta_info->file_name = (*OffsetMap)[file]->file_name;
		meta_info->file_size = (*OffsetMap)[file]->file_size;
		meta_info->is_directory = (*OffsetMap)[file]->is_directory;
		meta_info->mime_type = (*OffsetMap)[file]->mime_type;
		meta_info->mime_type_result = (*OffsetMap)[file]->mime_type_result;
		meta_info->offset = (*OffsetMap)[file]->offset;
		
		return true;
	}
	
	bool Utility::GetFileData(embed_util::FileMetaInfo *meta) {
		if(!embed_util::Utility::Load()) return false;
		base::MemoryMappedFile mmap_;
		if (!mmap_.Initialize(base::FilePath(Utility::GetContainer()))) {
			DLOG(ERROR) << "Failed to mmap application data (embed_utils.cc:134)";
			return false;
		}
		meta->data_size = 0;
		meta->data = (unsigned char *)malloc(CHUNK);
		int ret;
		{
			z_stream strm;
			strm.total_in = strm.avail_in = meta->file_size;
			strm.next_in = (unsigned char *)malloc(meta->file_size);
			memcpy(strm.next_in,mmap_.data()+meta->offset,meta->file_size);
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			ret = inflateInit2(&strm,(15 + 32));
			if (ret == Z_OK) {
				do {
					strm.avail_out = CHUNK;
					strm.next_out = meta->data + meta->data_size;
					ret = inflate(&strm, Z_NO_FLUSH);
					meta->data_size = meta->data_size + strm.avail_out;
					if(strm.avail_in != 0)
						meta->data = (unsigned char *)realloc(meta->data, meta->data_size + strm.avail_out);
				} while (strm.avail_in != 0 && ret != Z_STREAM_END);
				meta->data_size = strm.total_out;
				inflateEnd(&strm);
			}
		}
		return ret == Z_STREAM_END;
	}
}



