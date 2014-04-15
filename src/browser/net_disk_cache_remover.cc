// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net_disk_cache_remover.h"

#include "base/bind_helpers.h"
#include "base/synchronization/waitable_event.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "net/disk_cache/disk_cache.h"
#include "net/http/http_cache.h"
#include "net/http/http_transaction_factory.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request_context.h"
#include "net/base/completion_callback.h"

using content::BrowserThread;
using disk_cache::Backend;
using net::CompletionCallback;
using net::URLRequestContextGetter;

namespace {
// Everything is called and accessed on the IO thread.

void Noop(base::WaitableEvent* completion, int rv) {
  DCHECK(rv == net::OK);
  if (completion)
    completion->Signal();
}

void CallDoomAllEntries(Backend** backend, base::WaitableEvent* completion, int rv) {
  DCHECK(rv == net::OK);
  if (backend && *backend)
    (*backend)->DoomAllEntries(base::Bind(&Noop, completion));
}

void ClearHttpDiskCacheOfContext(URLRequestContextGetter* context_getter,
                                 base::WaitableEvent* completion) {
  typedef Backend* BackendPtr;  // Make line below easier to understand.
  BackendPtr* backend_ptr = new BackendPtr(NULL);
  CompletionCallback callback(base::Bind(&CallDoomAllEntries,
                                         base::Owned(backend_ptr),
                                         completion));

  int rv = context_getter->GetURLRequestContext()->
    http_transaction_factory()->GetCache()->GetBackend(backend_ptr, callback);

  // If not net::ERR_IO_PENDING, then backend pointer is updated but callback
  // is not called, so call it explicitly.
  if (rv != net::ERR_IO_PENDING)
    callback.Run(net::OK);
}

void ClearHttpDiskCacheOnIoThread(
    URLRequestContextGetter* main_context_getter,
    URLRequestContextGetter* media_context_getter,
    base::WaitableEvent* completion1,
    base::WaitableEvent* completion2) {
  ClearHttpDiskCacheOfContext(main_context_getter, completion1);
  ClearHttpDiskCacheOfContext(media_context_getter, completion2);
}

}  // namespace

namespace nw {

void RemoveHttpDiskCache(content::BrowserContext* browser_context,
                        int renderer_child_id) {
  URLRequestContextGetter* main_context_getter =
      browser_context->GetRequestContextForRenderProcess(renderer_child_id);
  URLRequestContextGetter* media_context_getter =
      browser_context->GetMediaRequestContextForRenderProcess(
          renderer_child_id);

  base::WaitableEvent comp1(false, false), comp2(false, false);
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&ClearHttpDiskCacheOnIoThread,
                 base::Unretained(main_context_getter),
                 base::Unretained(media_context_getter),
                 &comp1, &comp2));
  comp1.Wait();
  comp2.Wait();
}

}  // namespace nw
