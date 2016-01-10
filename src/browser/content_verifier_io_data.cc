// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/content_verifier_io_data.h"

#include "content/public/browser/browser_thread.h"

namespace nw {

ContentVerifierIOData::ExtensionData::ExtensionData(
    scoped_ptr<std::set<base::FilePath>> browser_image_paths,
    const base::Version& version) {
  this->browser_image_paths = browser_image_paths.Pass();
  this->version = version;
}

ContentVerifierIOData::ContentVerifierIOData() {
}

ContentVerifierIOData::ExtensionData::~ExtensionData() {
}

ContentVerifierIOData::~ContentVerifierIOData() {
}

void ContentVerifierIOData::AddData(const std::string& extension_id,
                                    scoped_ptr<ExtensionData> data) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  CHECK(data->browser_image_paths.get());
  data_map_[extension_id] = linked_ptr<ExtensionData>(data.release());
}

void ContentVerifierIOData::RemoveData(const std::string& extension_id) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  std::map<std::string, linked_ptr<ExtensionData> >::iterator found =
      data_map_.find(extension_id);
  if (found != data_map_.end())
    data_map_.erase(found);
}

void ContentVerifierIOData::Clear() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  data_map_.clear();
}

const ContentVerifierIOData::ExtensionData* ContentVerifierIOData::GetData(
    const std::string& extension_id) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  std::map<std::string, linked_ptr<ExtensionData> >::iterator found =
      data_map_.find(extension_id);
  if (found != data_map_.end())
    return found->second.get();
  else
    return NULL;
}

}  // namespace nw
