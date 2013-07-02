#include "content/nw/src/net/util/embed_utils.h"

#include "base/command_line.h"
#include "net/base/mime_util.h"
#include "third_party/node/deps/uv/include/uv.h"

namespace embed_util {

	FileMetaInfo::FileMetaInfo()
		: file_size(0),
		mime_type_result(false),
		file_exists(false),
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

		size_t size = 2*PATH_MAX;
		char* execPath = new char[size];
		if (uv_exepath(execPath, &size) == 0) path = base::FilePath::FromUTF8Unsafe(std::string(execPath, size));
		else path = base::FilePath(command_line->GetProgram());
#if defined(OS_MACOSX)
		path = path.DirName().DirName().Append("Resources").Append("Package");
#endif
		_path = path.AsUTF8Unsafe();
		cached = true;
		return _path;
	}

	int Utility::IndexOf(net::FileStream *haystack, const char* sign_needle) {
		const unsigned char* needle = (const unsigned char *)sign_needle;
		typedef std::vector<size_t> occtable_type;
		size_t needle_length = strlen(sign_needle);
		occtable_type occ(UCHAR_MAX+1, needle_length);
		size_t haystack_init_pos = haystack->SeekSync(net::Whence::FROM_CURRENT, 0);
		size_t haystack_length = haystack->Available();
		if(needle_length >= 1) {
			const size_t needle_length_minus_1 = needle_length-1;
			for(size_t a=0; a<needle_length_minus_1; ++a)
				occ[needle[a]] = needle_length_minus_1 - a;
		}
		if(needle_length > haystack_length) return -1;
		const size_t needle_length_minus_1 = needle_length-1;		
		const unsigned char last_needle_char = needle[needle_length_minus_1];
		size_t haystack_position=0;
		while(haystack_position <= haystack_length-needle_length) {
			unsigned char hay_chars[needle_length_minus_1+1];
			haystack->SeekSync(net::Whence::FROM_BEGIN, haystack_init_pos + haystack_position);
			haystack->ReadSync((char *)(&hay_chars[0]), needle_length_minus_1 + 1);
			if(last_needle_char == hay_chars[needle_length_minus_1] && std::memcmp(needle, hay_chars, needle_length_minus_1) == 0)
				return haystack_init_pos + haystack_position + needle_length_minus_1 + 1;
			haystack_position += occ[hay_chars[needle_length_minus_1]];
		}
		return -1;
	}
	
	bool Utility::Load() {
		static bool loaded = false;
		if(loaded==true) return true;
		loaded = true;
		std::map<std::string, embed_util::FileMetaInfo *> *OffsetMap = Utility::GetOffsetMap();
		std::string magickey = "\x20\x01\x77\xf3\x66\x31"; // [ 0x20, 0x01, 0x77, 0xf3, 0x66, 0x31, 0x00 ]
		net::FileStream *stream = new net::FileStream(NULL);
		int p1, p2;
		char *filename = new char[256];
		unsigned char *size = new unsigned char[8];
		
		if(stream->OpenSync(base::FilePath(Utility::GetContainer()), base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ) < 0) return false;
		if(stream->SeekSync(net::Whence::FROM_BEGIN, 0) < 0) return false;
		
		while(stream->Available() > 0)
		{
			p1 = Utility::IndexOf(stream, magickey.c_str());
			if(p1 == -1) return false;
			p2 = Utility::IndexOf(stream, magickey.c_str());
			if(p2 == -1) return false;

			if(static_cast<unsigned long>(p2-p1-magickey.size()) < sizeof(filename)) return false;
			
			if(stream->SeekSync(net::Whence::FROM_BEGIN, p1) < 0) return false;
			if(stream->ReadSync(filename, p2-p1-magickey.size()) < 0) return false;
			if(stream->SeekSync(net::Whence::FROM_BEGIN, p2) < 0) return false;
			if(stream->ReadSync((char *)size, 8) < 0) return false;
			std::string key = std::string(filename, p2-p1-magickey.size());
			(*OffsetMap)[key] = new embed_util::FileMetaInfo();
			(*OffsetMap)[key]->offset = p2 + 8;
			(*OffsetMap)[key]->file_exists = true;
			(*OffsetMap)[key]->file_size = (uint32_t)size[4] << 24 | (uint32_t)size[5] << 16 | (uint32_t)size[6] << 8 | (uint32_t)size[7];
			(*OffsetMap)[key]->is_directory = false;
			(*OffsetMap)[key]->mime_type_result = net::GetMimeTypeFromFile(base::FilePath::FromUTF8Unsafe(key), &(*OffsetMap)[key]->mime_type);
			(*OffsetMap)[key]->file_name = key;

			stream->SeekSync(net::Whence::FROM_CURRENT, (*OffsetMap)[key]->file_size);
		}
		return true;
	}
 
	bool Utility::GetFileInfo(std::string file, embed_util::FileMetaInfo* meta_info) {
		std::map<std::string, embed_util::FileMetaInfo *> *OffsetMap = Utility::GetOffsetMap();
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
}



