#include "content/nw/src/net/util/embed_utils.h"

#include "base/command_line.h"
#include "base/files/memory_mapped_file.h"
#include "net/base/mime_util.h"
#include "net/base/file_stream_whence.h"
#include "base/files/file_path.h"
#include "third_party/node/deps/uv/include/uv.h"

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

	std::string Utility::GetContainer() {
		static bool cached = false;
		static std::string _path;
		if(cached) return _path;

		CommandLine* command_line = CommandLine::ForCurrentProcess();
		base::FilePath path;
#if defined(OS_MACOSX)
		path = path.DirName().DirName().Append("Resources").Append("Package");
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
		_path = path.AsUTF8Unsafe();
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
		if(loaded==true) return loaded = true;
		std::map<std::string, embed_util::FileMetaInfo *> *OffsetMap = Utility::GetOffsetMap();
    std::shared_ptr<base::MemoryMappedFile> mmap_;
    size_t pos=0,a=0,b=0;
    mmap_.reset(new base::MemoryMappedFile);
    if (!mmap_->Initialize(base::FilePath::FromUTF8Unsafe(Utility::GetContainer()))) {
      DLOG(ERROR) << "Failed to mmap application data (embed_utils.cc:93)";
      mmap_.reset();
      return false;
    }
    const uint8* data = mmap_->data();
    const size_t len = mmap_->length();
		while(pos < len) {
			if( (a=Utility::IndexOf(reinterpret_cast<const unsigned char*>(data),len,pos)) < 0 ) return false;
			if( (b=Utility::IndexOf(reinterpret_cast<const unsigned char*>(data),len,pos+a)) < (MAGIC_SIZE+1) ) return false;
      if(a==b) return false;
      std::string key = std::string(reinterpret_cast<const char*>(data+pos+a),(b-MAGIC_SIZE));
			(*OffsetMap)[key] = new embed_util::FileMetaInfo();
			(*OffsetMap)[key]->offset = pos+a+b+8;
			(*OffsetMap)[key]->file_size = *reinterpret_cast<const unsigned int *>(data+pos+a+b);
			(*OffsetMap)[key]->mime_type_result = net::GetMimeTypeFromFile(base::FilePath::FromUTF8Unsafe(key), &(*OffsetMap)[key]->mime_type);
			(*OffsetMap)[key]->file_name = key;
      pos = (*OffsetMap)[key]->offset + (*OffsetMap)[key]->file_size;
		}
    mmap_.reset();
		return true;
	}
 
	bool Utility::GetFileInfo(std::string file, embed_util::FileMetaInfo* meta_info) {
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
	
	/*bool Utility::WriteTempFile(std::string filename, base::FilePath* path) {
		embed_util::FileMetaInfo info;
		file_util::GetTempDir(path);
		path->Append(base::FilePath::FromUTF8Unsafe(filename));
		if(Utility::GetFileInfo(filename, &info)) {
			char buf[info.file_size];
			net::FileStream *fstream = new net::FileStream(NULL);
			if(fstream->OpenSync(base::FilePath::FromUTF8Unsafe(Utility::GetContainer()), base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ) < 0) return false;
			fstream->SeekSync(net::Whence::FROM_BEGIN, info.offset);
			fstream->ReadSync(&buf, info.file_size);
			
			file_util::WriteFile(path, &buf, info.file_size);
			return true;
		}
		return false;
	}*/
}



