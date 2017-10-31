#include <stdio.h>

#include "base/at_exit.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/file_enumerator.h"
#include "base/i18n/icu_util.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/process/memory.h"
#include "base/strings/string_util.h"
#include "base/stl_util.h"
#include "crypto/secure_hash.h"
#include "crypto/sha2.h"
//#include "extensions/browser/content_hash_tree.h"
//#include "extensions/browser/computed_hashes.h"

#include "base/base64.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/values.h"


using crypto::kSHA256Length;
using crypto::SecureHash;


namespace {
const char kBlockHashesKey[] = "root_hash";
//const char kBlockSizeKey[] = "block_size";
const char kFileHashesKey[] = "files";
const char kPathKey[] = "path";
//const char kVersionKey[] = "version";
//const int kVersion = 2;

typedef std::set<base::FilePath> SortedFilePathSet;

bool MakePathAbsolute(base::FilePath* file_path) {
  DCHECK(file_path);

  base::FilePath current_directory;
  if (!base::GetCurrentDirectory(&current_directory))
    return false;

  if (file_path->IsAbsolute())
    return true;

  if (current_directory.empty()) {
    *file_path = base::MakeAbsoluteFilePath(*file_path);
    return true;
  }

  if (!current_directory.IsAbsolute())
    return false;

#ifdef OS_LINUX
  //linux might gives "/" as current_directory, return false
  if (current_directory.value().length() <= 1)
	return false;
#endif

  *file_path = current_directory.Append(*file_path);
  return true;
}

}  // namespace

class MyComputedHashes {
 public:
  class Writer {
   public:
    Writer();
    ~Writer();

    // Adds hashes for |relative_path|. Should not be called more than once
    // for a given |relative_path|.
    void AddHash(const base::FilePath& relative_path,
                 int block_size,
                 const std::string& hashes);

    bool WriteToFile(const base::FilePath& path);

   private:
    // Each element of this list contains the path and block hashes for one
    // file.
    std::unique_ptr<base::ListValue> file_list_;
  };

  // Computes the SHA256 hash of each |block_size| chunk in |contents|, placing
  // the results into |hashes|.
  static void ComputeHashesForContent(const std::string& contents,
                                      size_t block_size,
                                      std::vector<std::string>* hashes);
};

MyComputedHashes::Writer::Writer() : file_list_(new base::ListValue) {
}

MyComputedHashes::Writer::~Writer() {
}

void MyComputedHashes::Writer::AddHash(const base::FilePath& relative_path,
                                       int block_size,
                                       const std::string& root) {
  base::DictionaryValue* dict = new base::DictionaryValue();
  file_list_->Append(base::WrapUnique(dict));
  dict->SetString(kPathKey,
                  relative_path.NormalizePathSeparatorsTo('/').AsUTF8Unsafe());
  //dict->SetInteger(kBlockSizeKey, block_size);
  std::string encoded;
  base::Base64Encode(root, &encoded);
  base::ReplaceChars(encoded, "=", "", &encoded);
  base::ReplaceChars(encoded, "/", "_", &encoded);
  base::ReplaceChars(encoded, "+", "-", &encoded);
  dict->SetString(kBlockHashesKey, encoded);
}

bool MyComputedHashes::Writer::WriteToFile(const base::FilePath& path) {
  std::string json;
  base::DictionaryValue top_dictionary;
  top_dictionary.SetInteger("block_size", 4096);
  top_dictionary.SetInteger("hash_block_size", 4096);
  top_dictionary.SetString("format", "treehash");
  top_dictionary.Set(kFileHashesKey, file_list_.release());

  if (!base::JSONWriter::Write(top_dictionary, &json))
    return false;
  int written = base::WriteFile(path, json.data(), (int)json.size());
  if (static_cast<unsigned>(written) != json.size()) {
    LOG(ERROR) << "Error writing " << path.AsUTF8Unsafe()
               << " ; write result:" << written << " expected:" << json.size();
    return false;
  }
  return true;
}

void MyComputedHashes::ComputeHashesForContent(const std::string& contents,
                                             size_t block_size,
                                             std::vector<std::string>* hashes) {
  size_t offset = 0;
  // Even when the contents is empty, we want to output at least one hash
  // block (the hash of the empty string).
  do {
    const char* block_start = contents.data() + offset;
    DCHECK(offset <= contents.size());
    size_t bytes_to_read = std::min(contents.size() - offset, block_size);
    std::unique_ptr<crypto::SecureHash> hash(
        crypto::SecureHash::Create(crypto::SecureHash::SHA256));
    hash->Update(block_start, bytes_to_read);

    hashes->push_back(std::string());
    std::string* buffer = &(hashes->back());
    buffer->resize(crypto::kSHA256Length);
    hash->Finish(base::string_as_array(buffer), buffer->size());

    // If |contents| is empty, then we want to just exit here.
    if (bytes_to_read == 0)
      break;

    offset += bytes_to_read;
  } while (offset < contents.size());
}

std::string ComputeTreeHashRoot(const std::vector<std::string>& leaf_hashes,
                                int branch_factor) {
  if (leaf_hashes.empty() || branch_factor < 2)
    return std::string();

  // The nodes of the tree we're currently operating on.
  std::vector<std::string> current_nodes;

  // We avoid having to copy all of the input leaf nodes into |current_nodes|
  // by using a pointer. So the first iteration of the loop this points at
  // |leaf_hashes|, but thereafter it points at |current_nodes|.
  const std::vector<std::string>* current = &leaf_hashes;

  // Where we're inserting new hashes computed from the current level.
  std::vector<std::string> parent_nodes;

  while (current->size() > 1) {
    // Iterate over the current level of hashes, computing the hash of up to
    // |branch_factor| elements to form the hash of each parent node.
    std::vector<std::string>::const_iterator i = current->begin();
    while (i != current->end()) {
      std::unique_ptr<crypto::SecureHash> hash(
          crypto::SecureHash::Create(crypto::SecureHash::SHA256));
      for (int j = 0; j < branch_factor && i != current->end(); j++) {
        DCHECK_EQ(i->size(), crypto::kSHA256Length);
        hash->Update(i->data(), i->size());
        ++i;
      }
      parent_nodes.push_back(std::string(crypto::kSHA256Length, 0));
      std::string* output = &(parent_nodes.back());
      hash->Finish(base::string_as_array(output), output->size());
    }
    current_nodes.swap(parent_nodes);
    parent_nodes.clear();
    current = &current_nodes;
  }
  DCHECK_EQ(1u, current->size());
  return (*current)[0];
}

bool CreateHashes(const base::FilePath& hashes_file, const base::FilePath& work_path) {
  // Make sure the directory exists.
  if (!base::CreateDirectoryAndGetError(hashes_file.DirName(), NULL))
    return false;

  base::FilePath root_path(work_path);
  MakePathAbsolute(&root_path);
  base::FileEnumerator enumerator(root_path,
                                  true, /* recursive */
                                  base::FileEnumerator::FILES);
  // First discover all the file paths and put them in a sorted set.
  SortedFilePathSet paths;
  for (;;) {
    base::FilePath full_path = enumerator.Next();
    if (full_path.empty())
      break;
    paths.insert(full_path);
  }

  // Now iterate over all the paths in sorted order and compute the block hashes
  // for each one.
  MyComputedHashes::Writer writer;
  for (SortedFilePathSet::iterator i = paths.begin(); i != paths.end(); ++i) {
    const base::FilePath& full_path = *i;
    base::FilePath relative_path;
    root_path.AppendRelativePath(full_path, &relative_path);
    relative_path = relative_path.NormalizePathSeparatorsTo('/');

    std::string contents;
    if (!base::ReadFileToString(full_path, &contents)) {
      LOG(ERROR) << "Could not read " << full_path.MaybeAsASCII();
      continue;
    }

    // Iterate through taking the hash of each block of size (block_size_) of
    // the file.
    std::vector<std::string> hashes;
    MyComputedHashes::ComputeHashesForContent(contents, 4096, &hashes);
    std::string root =
      ComputeTreeHashRoot(hashes, 4096 / crypto::kSHA256Length);
    writer.AddHash(relative_path, 4096, root);
  }
  bool result = writer.WriteToFile(hashes_file);
  return result;
}

#if defined(OS_WIN)
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
  base::AtExitManager exit_manager;
  //base::i18n::InitializeICU();

  return CreateHashes(base::FilePath(FILE_PATH_LITERAL("payload.json")),
                      base::FilePath(FILE_PATH_LITERAL("."))) ? 1 : 0;
}
