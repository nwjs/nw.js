#ifndef CONTENT_NW_SRC_NET_UTIL_EMBED_REQUEST_JOB_H_
#define CONTENT_NW_SRC_NET_UTIL_EMBED_REQUEST_JOB_H_

#include <map>
#include <string>
#include <vector>

#include "net/base/file_stream.h"
#include "base/files/file_path.h"
#include "base/file_util.h"

namespace embed_util {
	class FileMetaInfo {
		public:
			FileMetaInfo();

			// Size of the file.
			uint32_t file_size;

			// Mime type info
			std::string mime_type;

			// File name
			std::string file_name;

			// Result returned from GetMimeTypeFromFile(), i.e. flag showing whether
			// obtaining of the mime type was successful.
			bool mime_type_result;

			// Flag showing whether the file exists.
			bool file_exists;

			// Flag showing whether the file name actually refers to a directory.
			bool is_directory;

			// Offset to the information in the file.
			int64 offset;
	};

	class Utility {
		public:
			static std::map<std::string, embed_util::FileMetaInfo *> *GetOffsetMap();
			
			static std::string GetContainer();

			static bool GetFileInfo(std::string path, embed_util::FileMetaInfo* meta_info);

			static int IndexOf(net::FileStream *input, const char* needle);

			static bool Load();
		
			//static bool WriteTempFile(std::string filename, base::FilePath* path);
	};
}

#endif // CONTENT_NW_SRC_NET_UTIL_EMBED_REQUEST_JOB_H_

