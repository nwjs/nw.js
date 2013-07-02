#ifndef CONTENT_NW_SRC_NET_EMBED_REQUEST_JOB_H_
#define CONTENT_NW_SRC_NET_EMBED_REQUEST_JOB_H_

#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#include "content/nw/src/net/util/embed_utils.h"
#include "net/base/net_export.h"
#include "net/http/http_byte_range.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_job.h"


namespace base {
  struct PlatformFileInfo;
}
namespace file_util {
  struct FileInfo;
}
namespace net {
  class FileStream;
}

namespace nw {
  using namespace net;

  class NET_EXPORT EmbedRequestJob : public net::URLRequestJob {
  public:
    EmbedRequestJob(URLRequest* request,
                       NetworkDelegate* network_delegate,
                       std::string file);
    
    static EmbedRequestJob* Factory(URLRequest* request, NetworkDelegate* network_delegate);
    static bool EmbedURLToFilePath(const GURL& url, base::FilePath* path);
    // net::URLRequestJob methods.
    virtual void Start() OVERRIDE;
    virtual void Kill() OVERRIDE;
    virtual bool ReadRawData(net::IOBuffer* dest, int dest_size, int* bytes_read) OVERRIDE;
    virtual Filter* SetupFilter() const OVERRIDE;
    virtual bool GetMimeType(std::string* mime_type) const OVERRIDE;
    virtual void SetExtraRequestHeaders(const HttpRequestHeaders& headers) OVERRIDE;
    
  private:
    int64 remaining_bytes_;
    std::string file_;
    scoped_ptr<FileStream> stream_;
    base::WeakPtrFactory<EmbedRequestJob> weak_factory_;
    embed_util::FileMetaInfo meta_info_;
    HttpByteRange byte_range_;

    
    static void FetchMetaInfo(const std::string& name, embed_util::FileMetaInfo* meta_info);
				static void FindFileOffset(const std::string &file_, net::FileStream *stream, embed_util::FileMetaInfo* meta_info_);

				void DidFindFileOffset();
    void DidFetchMetaInfo(const embed_util::FileMetaInfo* meta_info);
    void DidOpen(int result);
    void DidSeek(int64 result);
    void DidRead(int result);
    
    virtual ~EmbedRequestJob();

    DISALLOW_COPY_AND_ASSIGN(EmbedRequestJob);
  };
  
}  // namespace nw

#endif  // CONTENT_NW_SRC_NET_EMBED_REQUEST_JOB_H_

