// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/net/embed_request_job.h"

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/logging.h"
#include "base/platform_file.h"
#include "base/files/file_path.h"
#include "base/strings/string_util.h"
#include "base/synchronization/lock.h"
#include "base/threading/worker_pool.h"
#include "base/threading/thread_restrictions.h"
#include "googleurl/src/gurl.h"
#include "net/base/escape.h"
#include "net/base/file_stream.h"
#include "net/base/io_buffer.h"
#include "net/base/mime_util.h"
#include "net/base/net_errors.h"
#include "net/base/net_util.h"
#include "net/http/http_util.h"
#include "net/http/http_response_headers.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_error_job.h"
#include "net/url_request/url_request_file_dir_job.h"
#include "net/url_request/url_request_job.h"
using namespace base;
namespace nw {
  using namespace net;
  using namespace embed_util;

	// static
	std::string EmbedRequestJob::EmbedURLToFilePath(const GURL& url) {
		if (!url.is_valid()) return NULL;
		std::string old_path = url.path();
		if (old_path.empty()) return NULL;
		old_path = UnescapeURLComponent(old_path, UnescapeRule::SPACES | UnescapeRule::URL_SPECIAL_CHARS);
		std::string new_path;
		do {
		  new_path = old_path;
		  ReplaceSubstringsAfterOffset(&new_path, 0, "//", "/");
		  old_path.swap(new_path);
		} while (new_path != old_path);
		return old_path;
	}

	// static
	EmbedRequestJob* EmbedRequestJob::Factory(URLRequest* request, NetworkDelegate* network_delegate) {
		return new EmbedRequestJob(request,network_delegate,EmbedURLToFilePath(request->url()));
	}
	
	
	EmbedRequestJob::EmbedRequestJob(URLRequest* request,  NetworkDelegate* network_delegate, std::string file)
		: URLRequestJob(request, network_delegate),
		remaining_bytes_(0),
		file_(file),
		stream_(new FileStream(NULL)),
		weak_factory_(this) {
	}
  
	EmbedRequestJob::~EmbedRequestJob() {}

	void EmbedRequestJob::Start() {
		embed_util::FileMetaInfo* meta_info = new embed_util::FileMetaInfo();
		base::WorkerPool::PostTaskAndReply(
			 FROM_HERE,
			 base::Bind(&EmbedRequestJob::FetchMetaInfo, file_, base::Unretained(meta_info)),
			 base::Bind(&EmbedRequestJob::DidFetchMetaInfo, weak_factory_.GetWeakPtr(), base::Owned(meta_info)),
			 true);
	}

	void EmbedRequestJob::FetchMetaInfo(const std::string& path, embed_util::FileMetaInfo* meta_info) {
		Utility::GetFileInfo(path, meta_info);
	}

	void EmbedRequestJob::DidFetchMetaInfo(const embed_util::FileMetaInfo* meta_info) {
		meta_info_ = *meta_info;
		if (!meta_info_.file_exists) {
			DidOpen(ERR_FILE_NOT_FOUND);
			return;
		}
		int rv = stream_->Open(base::FilePath(Utility::GetContainer()),
							   base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ | base::PLATFORM_FILE_ASYNC,
							   base::Bind(&EmbedRequestJob::DidOpen, weak_factory_.GetWeakPtr()));
		if (rv != ERR_IO_PENDING) DidOpen(rv);
	}

	void EmbedRequestJob::DidOpen(int result) {
		if (result != OK) {
			URLRequestJob::NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, result));
			return;
		}
		if (!byte_range_.ComputeBounds(meta_info_.file_size)) {
			URLRequestJob::NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, ERR_REQUEST_RANGE_NOT_SATISFIABLE));
			return;
		}
		remaining_bytes_ = byte_range_.last_byte_position() - byte_range_.first_byte_position() + 1;
		if (remaining_bytes_ < 1 || stream_->Seek(FROM_BEGIN,
						  byte_range_.first_byte_position() + meta_info_.offset,
						  base::Bind(&EmbedRequestJob::DidSeek, weak_factory_.GetWeakPtr())) != ERR_IO_PENDING)
			DidSeek(-1);
	}
	
	void EmbedRequestJob::DidSeek(int64 result) {
		if (result != (byte_range_.first_byte_position() + meta_info_.offset)) {
			URLRequestJob::NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, ERR_REQUEST_RANGE_NOT_SATISFIABLE));
			return;
		}
		URLRequestJob::set_expected_content_size(remaining_bytes_);
		URLRequestJob::NotifyHeadersComplete();
	}
	
	bool EmbedRequestJob::ReadRawData(IOBuffer* dest, int dest_size, int *bytes_read) {
		DCHECK_NE(dest_size, 0);
		DCHECK(bytes_read);
		DCHECK_GE(remaining_bytes_, 0);
		if (remaining_bytes_ < dest_size) dest_size = static_cast<int>(remaining_bytes_);
		if (!dest_size) {
			*bytes_read = 0;
			return true;
		}
		int rv = stream_->Read(dest, dest_size, base::Bind(&EmbedRequestJob::DidRead, weak_factory_.GetWeakPtr()));
		if (rv >= 0) {
			// Data is immediately available.
			*bytes_read = rv;
			remaining_bytes_ -= rv;
			DCHECK_GE(remaining_bytes_, 0);
			return true;
		}
		if (rv == ERR_IO_PENDING) URLRequestJob::SetStatus(URLRequestStatus(URLRequestStatus::IO_PENDING, 0));
		else URLRequestJob::NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, rv));
		return false;
	}
	
	void EmbedRequestJob::DidRead(int result) {
		if (result > 0) URLRequestJob::SetStatus(URLRequestStatus());  // Clear the IO_PENDING status
		else if (result == 0) URLRequestJob::NotifyDone(URLRequestStatus());
		else URLRequestJob::NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, result));
		remaining_bytes_ -= result;
		DCHECK_GE(remaining_bytes_, 0);
		URLRequestJob::NotifyReadComplete(result);
	}

	bool EmbedRequestJob::GetMimeType(std::string* mime_type) const {
		if(!meta_info_.mime_type_result) {
			*mime_type = "application/octet-stream";
			return false;
		}
		*mime_type = meta_info_.mime_type;
		return true;
	}
	
	void EmbedRequestJob::Kill() {
		stream_.reset();
		weak_factory_.InvalidateWeakPtrs();
		URLRequestJob::Kill();
	}

	Filter* EmbedRequestJob::SetupFilter() const {
		return Filter::GZipFactory(); //return LowerCaseEqualsASCII(base::FilePath(file_).Extension(), ".svgz") ? Filter::GZipFactory() : NULL;
	}
  
	void EmbedRequestJob::SetExtraRequestHeaders(const HttpRequestHeaders& headers) {
		std::string range_header;
		if (headers.GetHeader(HttpRequestHeaders::kRange, &range_header)) {
			std::vector<HttpByteRange> ranges;
			if (HttpUtil::ParseRangeHeader(range_header, &ranges)) {
				if (ranges.size() == 1) byte_range_ = ranges[0];
				else URLRequestJob::NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, ERR_REQUEST_RANGE_NOT_SATISFIABLE));
			}
		}
	}
	
	bool EmbedRequestJob::IsRedirectResponse(GURL* location,  int* http_status_code) {
		return false;
	}
}

