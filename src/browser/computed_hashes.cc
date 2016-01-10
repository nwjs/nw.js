// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/computed_hashes.h"

#include "base/base64.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/stl_util.h"
#include "base/values.h"
#include "crypto/secure_hash.h"
#include "crypto/sha2.h"

namespace {
const char kBlockHashesKey[] = "block_hashes";
const char kBlockSizeKey[] = "block_size";
const char kFileHashesKey[] = "file_hashes";
const char kPathKey[] = "path";
const char kVersionKey[] = "version";
const int kVersion = 2;
}  // namespace

namespace nw {

ComputedHashes::Reader::Reader() {
}

ComputedHashes::Reader::~Reader() {
}

bool ComputedHashes::Reader::InitFromFile(const base::FilePath& path) {
  std::string contents;
  if (!base::ReadFileToString(path, &contents))
    return false;

  base::DictionaryValue* top_dictionary = NULL;
  scoped_ptr<base::Value> value(base::JSONReader::Read(contents));
  if (!value.get() || !value->GetAsDictionary(&top_dictionary))
    return false;

  // For now we don't support forwards or backwards compatability in the
  // format, so we return false on version mismatch.
  int version = 0;
  if (!top_dictionary->GetInteger(kVersionKey, &version) || version != kVersion)
    return false;

  base::ListValue* all_hashes = NULL;
  if (!top_dictionary->GetList(kFileHashesKey, &all_hashes))
    return false;

  for (size_t i = 0; i < all_hashes->GetSize(); i++) {
    base::DictionaryValue* dictionary = NULL;
    if (!all_hashes->GetDictionary(i, &dictionary))
      return false;

    std::string relative_path_utf8;
    if (!dictionary->GetString(kPathKey, &relative_path_utf8))
      return false;

    int block_size;
    if (!dictionary->GetInteger(kBlockSizeKey, &block_size))
      return false;
    if (block_size <= 0 || ((block_size % 1024) != 0)) {
      LOG(ERROR) << "Invalid block size: " << block_size;
      block_size = 0;
      return false;
    }

    base::ListValue* hashes_list = NULL;
    if (!dictionary->GetList(kBlockHashesKey, &hashes_list))
      return false;

    base::FilePath relative_path =
        base::FilePath::FromUTF8Unsafe(relative_path_utf8);
    relative_path = relative_path.NormalizePathSeparatorsTo('/');

    data_[relative_path] = HashInfo(block_size, std::vector<std::string>());
    std::vector<std::string>* hashes = &(data_[relative_path].second);

    for (size_t j = 0; j < hashes_list->GetSize(); j++) {
      std::string encoded;
      if (!hashes_list->GetString(j, &encoded))
        return false;

      hashes->push_back(std::string());
      std::string* decoded = &hashes->back();
      if (!base::Base64Decode(encoded, decoded)) {
        hashes->clear();
        return false;
      }
    }
  }
  return true;
}

bool ComputedHashes::Reader::GetHashes(const base::FilePath& relative_path,
                                       int* block_size,
                                       std::vector<std::string>* hashes) {
  base::FilePath path = relative_path.NormalizePathSeparatorsTo('/');
  std::map<base::FilePath, HashInfo>::iterator i = data_.find(path);
  if (i == data_.end()) {
    // If we didn't find the entry using exact match, it's possible the
    // developer is using a path with some letters in the incorrect case, which
    // happens to work on windows/osx. So try doing a linear scan to look for a
    // case-insensitive match. In practice most extensions don't have that big
    // a list of files so the performance penalty is probably not too big
    // here. Also for crbug.com/29941 we plan to start warning developers when
    // they are making this mistake, since their extension will be broken on
    // linux/chromeos.
    for (i = data_.begin(); i != data_.end(); ++i) {
      const base::FilePath& entry = i->first;
      if (base::FilePath::CompareEqualIgnoreCase(entry.value(), path.value()))
        break;
    }
    if (i == data_.end())
      return false;
  }
  HashInfo& info = i->second;
  *block_size = info.first;
  *hashes = info.second;
  return true;
}

ComputedHashes::Writer::Writer() : file_list_(new base::ListValue) {
}

ComputedHashes::Writer::~Writer() {
}

void ComputedHashes::Writer::AddHashes(const base::FilePath& relative_path,
                                       int block_size,
                                       const std::vector<std::string>& hashes) {
  base::DictionaryValue* dict = new base::DictionaryValue();
  base::ListValue* block_hashes = new base::ListValue();
  file_list_->Append(dict);
  dict->SetString(kPathKey,
                  relative_path.NormalizePathSeparatorsTo('/').AsUTF8Unsafe());
  dict->SetInteger(kBlockSizeKey, block_size);
  dict->Set(kBlockHashesKey, block_hashes);

  for (std::vector<std::string>::const_iterator i = hashes.begin();
       i != hashes.end();
       ++i) {
    std::string encoded;
    base::Base64Encode(*i, &encoded);
    block_hashes->AppendString(encoded);
  }
}

bool ComputedHashes::Writer::WriteToFile(const base::FilePath& path) {
  std::string json;
  base::DictionaryValue top_dictionary;
  top_dictionary.SetInteger(kVersionKey, kVersion);
  top_dictionary.Set(kFileHashesKey, file_list_.release());

  if (!base::JSONWriter::Write(&top_dictionary, &json))
    return false;
  int written = base::WriteFile(path, json.data(), json.size());
  if (static_cast<unsigned>(written) != json.size()) {
    LOG(ERROR) << "Error writing " << path.AsUTF8Unsafe()
               << " ; write result:" << written << " expected:" << json.size();
    return false;
  }
  return true;
}

void ComputedHashes::ComputeHashesForContent(const std::string& contents,
                                             size_t block_size,
                                             std::vector<std::string>* hashes) {
  size_t offset = 0;
  // Even when the contents is empty, we want to output at least one hash
  // block (the hash of the empty string).
  do {
    const char* block_start = contents.data() + offset;
    DCHECK(offset <= contents.size());
    size_t bytes_to_read = std::min(contents.size() - offset, block_size);
    scoped_ptr<crypto::SecureHash> hash(
        crypto::SecureHash::Create(crypto::SecureHash::SHA256));
    hash->Update(block_start, bytes_to_read);

    hashes->push_back(std::string());
    std::string* buffer = &(hashes->back());
    buffer->resize(crypto::kSHA256Length);
    hash->Finish(string_as_array(buffer), buffer->size());

    // If |contents| is empty, then we want to just exit here.
    if (bytes_to_read == 0)
      break;

    offset += bytes_to_read;
  } while (offset < contents.size());
}

}  // namespace nw
