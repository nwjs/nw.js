// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/file_select_helper.h"

#include <string>
#include <utility>

#include "base/bind.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
//#include "chrome/browser/browser_process.h"
#include "chrome/browser/platform_util.h"
//#include "chrome/browser/profiles/profile.h"
//#include "chrome/browser/profiles/profile_manager.h"
// #include "chrome/browser/ui/browser.h"
// #include "chrome/browser/ui/browser_list.h"
// #include "chrome/browser/ui/chrome_select_file_policy.h"
#include "grit/nw_resources.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/file_chooser_file_info.h"
#include "content/public/common/file_chooser_params.h"
#include "net/base/mime_util.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/shell_dialogs/selected_file_info.h"

#if defined(OS_CHROMEOS)
#include "chrome/browser/chromeos/file_manager/fileapi_util.h"
#include "content/public/browser/site_instance.h"
#endif

using content::BrowserThread;
using content::FileChooserParams;
using content::RenderViewHost;
using content::RenderWidgetHost;
using content::WebContents;

namespace {

// There is only one file-selection happening at any given time,
// so we allocate an enumeration ID for that purpose.  All IDs from
// the renderer must start at 0 and increase.
const int kFileSelectEnumerationId = -1;

// Converts a list of FilePaths to a list of ui::SelectedFileInfo.
std::vector<ui::SelectedFileInfo> FilePathListToSelectedFileInfoList(
    const std::vector<base::FilePath>& paths) {
  std::vector<ui::SelectedFileInfo> selected_files;
  for (size_t i = 0; i < paths.size(); ++i) {
    selected_files.push_back(
        ui::SelectedFileInfo(paths[i], paths[i]));
  }
  return selected_files;
}

void DeleteFiles(const std::vector<base::FilePath>& paths) {
  for (auto& file_path : paths)
    base::DeleteFile(file_path, false);
}

}  // namespace

struct FileSelectHelper::ActiveDirectoryEnumeration {
  ActiveDirectoryEnumeration() : rvh_(NULL) {}

  scoped_ptr<DirectoryListerDispatchDelegate> delegate_;
  scoped_ptr<net::DirectoryLister> lister_;
  RenderViewHost* rvh_;
  std::vector<base::FilePath> results_;
};

FileSelectHelper::FileSelectHelper()
    :
      render_view_host_(NULL),
      web_contents_(NULL),
      select_file_dialog_(),
      select_file_types_(),
      dialog_type_(ui::SelectFileDialog::SELECT_OPEN_FILE),
      dialog_mode_(FileChooserParams::Open) {
}

FileSelectHelper::~FileSelectHelper() {
  // There may be pending file dialogs, we need to tell them that we've gone
  // away so they don't try and call back to us.
  if (select_file_dialog_.get())
    select_file_dialog_->ListenerDestroyed();

  // Stop any pending directory enumeration, prevent a callback, and free
  // allocated memory.
  std::map<int, ActiveDirectoryEnumeration*>::iterator iter;
  for (iter = directory_enumerations_.begin();
       iter != directory_enumerations_.end();
       ++iter) {
    iter->second->lister_.reset();
    delete iter->second;
  }
}

void FileSelectHelper::DirectoryListerDispatchDelegate::OnListFile(
    const net::DirectoryLister::DirectoryListerData& data) {
  parent_->OnListFile(id_, data);
}

void FileSelectHelper::DirectoryListerDispatchDelegate::OnListDone(int error) {
  parent_->OnListDone(id_, error);
}

void FileSelectHelper::FileSelected(const base::FilePath& path,
                                    int index, void* params) {
  FileSelectedWithExtraInfo(ui::SelectedFileInfo(path, path), index, params);
}

void FileSelectHelper::FileSelectedWithExtraInfo(
    const ui::SelectedFileInfo& file,
    int index,
    void* params) {

  if (!render_view_host_) {
    RunFileChooserEnd();
    return;
  }

  const base::FilePath& path = file.local_path;
  if (dialog_type_ == ui::SelectFileDialog::SELECT_UPLOAD_FOLDER &&
      extract_directory_) {
    StartNewEnumeration(path, kFileSelectEnumerationId, render_view_host_);
    return;
  }

  std::vector<ui::SelectedFileInfo> files;
  files.push_back(file);

#if defined(OS_MAC) && !defined(OS_IOS)
  content::BrowserThread::PostTask(
      content::BrowserThread::FILE_USER_BLOCKING,
      FROM_HERE,
      base::Bind(&FileSelectHelper::ProcessSelectedFilesMac, this, files));
#else
  NotifyRenderViewHostAndEnd(files);
#endif  // defined(OS_MAC) && !defined(OS_IOS)
}

void FileSelectHelper::MultiFilesSelected(
    const std::vector<base::FilePath>& files,
    void* params) {
  std::vector<ui::SelectedFileInfo> selected_files =
      FilePathListToSelectedFileInfoList(files);

  MultiFilesSelectedWithExtraInfo(selected_files, params);
}

void FileSelectHelper::MultiFilesSelectedWithExtraInfo(
    const std::vector<ui::SelectedFileInfo>& files,
    void* params) {

#if defined(OS_MAC) && !defined(OS_IOS)
  content::BrowserThread::PostTask(
      content::BrowserThread::FILE_USER_BLOCKING,
      FROM_HERE,
      base::Bind(&FileSelectHelper::ProcessSelectedFilesMac, this, files));
#else
  NotifyRenderViewHostAndEnd(files);
#endif  // defined(OS_MAC) && !defined(OS_IOS)
}

void FileSelectHelper::FileSelectionCanceled(void* params) {
  NotifyRenderViewHostAndEnd(std::vector<ui::SelectedFileInfo>());
}

void FileSelectHelper::StartNewEnumeration(const base::FilePath& path,
                                           int request_id,
                                           RenderViewHost* render_view_host) {
  scoped_ptr<ActiveDirectoryEnumeration> entry(new ActiveDirectoryEnumeration);
  entry->rvh_ = render_view_host;
  entry->delegate_.reset(new DirectoryListerDispatchDelegate(this, request_id));
  entry->lister_.reset(new net::DirectoryLister(path,
                                                true,
                                                net::DirectoryLister::NO_SORT,
                                                entry->delegate_.get()));
  if (!entry->lister_->Start()) {
    if (request_id == kFileSelectEnumerationId)
      FileSelectionCanceled(NULL);
    else
      render_view_host->DirectoryEnumerationFinished(request_id,
                                                     entry->results_);
  } else {
    directory_enumerations_[request_id] = entry.release();
  }
}

void FileSelectHelper::OnListFile(
    int id,
    const net::DirectoryLister::DirectoryListerData& data) {
  ActiveDirectoryEnumeration* entry = directory_enumerations_[id];

  // Directory upload only cares about files.
  if (data.info.IsDirectory())
    return;

  entry->results_.push_back(data.path);
}

void FileSelectHelper::OnListDone(int id, int error) {
  // This entry needs to be cleaned up when this function is done.
  scoped_ptr<ActiveDirectoryEnumeration> entry(directory_enumerations_[id]);
  directory_enumerations_.erase(id);
  if (!entry->rvh_)
    return;
  if (error) {
    FileSelectionCanceled(NULL);
    return;
  }

  std::vector<ui::SelectedFileInfo> selected_files =
      FilePathListToSelectedFileInfoList(entry->results_);

  if (id == kFileSelectEnumerationId) {
    NotifyRenderViewHostAndEnd(selected_files);
  } else {
    entry->rvh_->DirectoryEnumerationFinished(id, entry->results_);
    EnumerateDirectoryEnd();
  }
}

void FileSelectHelper::NotifyRenderViewHostAndEnd(
    const std::vector<ui::SelectedFileInfo>& files) {
  if (!render_view_host_) {
    RunFileChooserEnd();
    return;
  }

#if defined(OS_CHROMEOS)
  if (!files.empty()) {
    if (!IsValidProfile(profile_)) {
      RunFileChooserEnd();
      return;
    }
    // Converts |files| into FileChooserFileInfo with handling of non-native
    // files.
    file_manager::util::ConvertSelectedFileInfoListToFileChooserFileInfoList(
        file_manager::util::GetFileSystemContextForRenderViewHost(
            profile_, render_view_host_),
        web_contents_->GetSiteInstance()->GetSiteURL(),
        files,
        base::Bind(
            &FileSelectHelper::NotifyRenderViewHostAndEndAfterConversion,
            this));
    return;
  }
#endif  // defined(OS_CHROMEOS)

  std::vector<content::FileChooserFileInfo> chooser_files;
  for (const auto& file : files) {
    content::FileChooserFileInfo chooser_file;
    chooser_file.file_path = file.local_path;
    chooser_file.display_name = file.display_name;
    chooser_files.push_back(chooser_file);
  }

  NotifyRenderViewHostAndEndAfterConversion(chooser_files);
}

void FileSelectHelper::NotifyRenderViewHostAndEndAfterConversion(
    const std::vector<content::FileChooserFileInfo>& list) {
  if (render_view_host_)
    render_view_host_->FilesSelectedInChooser(list, dialog_mode_);

  // No members should be accessed from here on.
  RunFileChooserEnd();
}

void FileSelectHelper::DeleteTemporaryFiles() {
  BrowserThread::PostTask(BrowserThread::FILE,
                          FROM_HERE,
                          base::Bind(&DeleteFiles, temporary_files_));
  temporary_files_.clear();
}

scoped_ptr<ui::SelectFileDialog::FileTypeInfo>
FileSelectHelper::GetFileTypesFromAcceptType(
    const std::vector<base::string16>& accept_types) {
  scoped_ptr<ui::SelectFileDialog::FileTypeInfo> base_file_type(
      new ui::SelectFileDialog::FileTypeInfo());
  if (accept_types.empty())
    return base_file_type.Pass();

  // Create FileTypeInfo and pre-allocate for the first extension list.
  scoped_ptr<ui::SelectFileDialog::FileTypeInfo> file_type(
      new ui::SelectFileDialog::FileTypeInfo(*base_file_type));
  file_type->include_all_files = true;
  file_type->extensions.resize(1);
  std::vector<base::FilePath::StringType>* extensions =
      &file_type->extensions.back();

  // Find the corresponding extensions.
  int valid_type_count = 0;
  int description_id = 0;
  for (size_t i = 0; i < accept_types.size(); ++i) {
    std::string ascii_type = base::UTF16ToASCII(accept_types[i]);
    if (!IsAcceptTypeValid(ascii_type))
      continue;

    size_t old_extension_size = extensions->size();
    if (ascii_type[0] == '.') {
      // If the type starts with a period it is assumed to be a file extension
      // so we just have to add it to the list.
      base::FilePath::StringType ext(ascii_type.begin(), ascii_type.end());
      extensions->push_back(ext.substr(1));
    } else {
      if (ascii_type == "image/*")
        description_id = IDS_IMAGE_FILES;
      else if (ascii_type == "audio/*")
        description_id = IDS_AUDIO_FILES;
      else if (ascii_type == "video/*")
        description_id = IDS_VIDEO_FILES;

      net::GetExtensionsForMimeType(ascii_type, extensions);
    }

    if (extensions->size() > old_extension_size)
      valid_type_count++;
  }

  // If no valid extension is added, bail out.
  if (valid_type_count == 0)
    return base_file_type.Pass();

  // Use a generic description "Custom Files" if either of the following is
  // true:
  // 1) There're multiple types specified, like "audio/*,video/*"
  // 2) There're multiple extensions for a MIME type without parameter, like
  //    "ehtml,shtml,htm,html" for "text/html". On Windows, the select file
  //    dialog uses the first extension in the list to form the description,
  //    like "EHTML Files". This is not what we want.
  if (valid_type_count > 1 ||
      (valid_type_count == 1 && description_id == 0 && extensions->size() > 1))
    description_id = IDS_CUSTOM_FILES;

  if (description_id) {
    file_type->extension_description_overrides.push_back(
        l10n_util::GetStringUTF16(description_id));
  }

  return file_type.Pass();
}

// static
void FileSelectHelper::RunFileChooser(content::WebContents* tab,
                                      const FileChooserParams& params) {
  // FileSelectHelper will keep itself alive until it sends the result message.
  scoped_refptr<FileSelectHelper> file_select_helper(
      new FileSelectHelper());
  file_select_helper->extract_directory_ = params.extract_directory;
  file_select_helper->RunFileChooser(tab->GetRenderViewHost(), tab, params);
}

// static
void FileSelectHelper::EnumerateDirectory(content::WebContents* tab,
                                          int request_id,
                                          const base::FilePath& path) {
  // FileSelectHelper will keep itself alive until it sends the result message.
  scoped_refptr<FileSelectHelper> file_select_helper(
      new FileSelectHelper());
  file_select_helper->EnumerateDirectory(
      request_id, tab->GetRenderViewHost(), path);
}

void FileSelectHelper::RunFileChooser(RenderViewHost* render_view_host,
                                      content::WebContents* web_contents,
                                      const FileChooserParams& params) {
  DCHECK(!render_view_host_);
  DCHECK(!web_contents_);
  render_view_host_ = render_view_host;
  web_contents_ = web_contents;
  notification_registrar_.RemoveAll();
  notification_registrar_.Add(this,
                              content::NOTIFICATION_RENDER_VIEW_HOST_CHANGED,
                              content::Source<WebContents>(web_contents_));
  notification_registrar_.Add(
      this,
      content::NOTIFICATION_RENDER_WIDGET_HOST_DESTROYED,
      content::Source<RenderWidgetHost>(render_view_host_));
  notification_registrar_.Add(
      this, content::NOTIFICATION_WEB_CONTENTS_DESTROYED,
      content::Source<WebContents>(web_contents_));

  BrowserThread::PostTask(
      BrowserThread::FILE, FROM_HERE,
      base::Bind(&FileSelectHelper::RunFileChooserOnFileThread, this, params));

  // Because this class returns notifications to the RenderViewHost, it is
  // difficult for callers to know how long to keep a reference to this
  // instance. We AddRef() here to keep the instance alive after we return
  // to the caller, until the last callback is received from the file dialog.
  // At that point, we must call RunFileChooserEnd().
  AddRef();
}

void FileSelectHelper::RunFileChooserOnFileThread(
    const FileChooserParams& params) {
  select_file_types_ = GetFileTypesFromAcceptType(params.accept_types);
  select_file_types_->support_drive = !params.need_local_path;

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&FileSelectHelper::RunFileChooserOnUIThread, this, params));
}

void FileSelectHelper::RunFileChooserOnUIThread(
    const FileChooserParams& params) {
  if (!render_view_host_ || !web_contents_) {
    // If the renderer was destroyed before we started, just cancel the
    // operation.
    RunFileChooserEnd();
    return;
  }

  select_file_dialog_ = ui::SelectFileDialog::Create(
      this, NULL);
  if (!select_file_dialog_.get())
    return;

  dialog_mode_ = params.mode;
  switch (params.mode) {
    case FileChooserParams::Open:
      dialog_type_ = ui::SelectFileDialog::SELECT_OPEN_FILE;
      break;
    case FileChooserParams::OpenMultiple:
      dialog_type_ = ui::SelectFileDialog::SELECT_OPEN_MULTI_FILE;
      break;
    case FileChooserParams::UploadFolder:
      dialog_type_ = ui::SelectFileDialog::SELECT_FOLDER;
      break;
    case FileChooserParams::Save:
      dialog_type_ = ui::SelectFileDialog::SELECT_SAVEAS_FILE;
      break;
    default:
      // Prevent warning.
      dialog_type_ = ui::SelectFileDialog::SELECT_OPEN_FILE;
      NOTREACHED();
  }

  base::FilePath default_file_name = params.default_file_name;
  base::FilePath working_path      = params.initial_path;

  gfx::NativeWindow owning_window =
      platform_util::GetTopLevel(render_view_host_->GetView()->GetNativeView());

#if defined(OS_ANDROID)
  // Android needs the original MIME types and an additional capture value.
  std::pair<std::vector<base::string16>, bool> accept_types =
      std::make_pair(params.accept_types, params.capture);
#endif

  select_file_dialog_->SelectFile(
      dialog_type_,
      params.title,
      default_file_name,
      select_file_types_.get(),
      select_file_types_.get() && !select_file_types_->extensions.empty()
          ? 1
          : 0,  // 1-based index of default extension to show.
      base::FilePath::StringType(),
      owning_window,
#if defined(OS_ANDROID)
      &accept_types);
#else
  NULL,
    working_path);
#endif

  select_file_types_.reset();
}

// This method is called when we receive the last callback from the file
// chooser dialog. Perform any cleanup and release the reference we added
// in RunFileChooser().
void FileSelectHelper::RunFileChooserEnd() {
  // If there are temporary files, then this instance needs to stick around
  // until web_contents_ is destroyed, so that this instance can delete the
  // temporary files.
  if (!temporary_files_.empty())
    return;

  render_view_host_ = NULL;
  web_contents_ = NULL;
  Release();
}

void FileSelectHelper::EnumerateDirectory(int request_id,
                                          RenderViewHost* render_view_host,
                                          const base::FilePath& path) {

  // Because this class returns notifications to the RenderViewHost, it is
  // difficult for callers to know how long to keep a reference to this
  // instance. We AddRef() here to keep the instance alive after we return
  // to the caller, until the last callback is received from the enumeration
  // code. At that point, we must call EnumerateDirectoryEnd().
  AddRef();
  StartNewEnumeration(path, request_id, render_view_host);
}

// This method is called when we receive the last callback from the enumeration
// code. Perform any cleanup and release the reference we added in
// EnumerateDirectory().
void FileSelectHelper::EnumerateDirectoryEnd() {
  Release();
}

void FileSelectHelper::Observe(int type,
                               const content::NotificationSource& source,
                               const content::NotificationDetails& details) {
  switch (type) {
    case content::NOTIFICATION_RENDER_WIDGET_HOST_DESTROYED: {
      DCHECK(content::Source<RenderWidgetHost>(source).ptr() ==
             render_view_host_);
      render_view_host_ = NULL;
      break;
    }

    case content::NOTIFICATION_WEB_CONTENTS_DESTROYED: {
      DCHECK(content::Source<WebContents>(source).ptr() == web_contents_);
      web_contents_ = NULL;
    }

    // Intentional fall through.
    case content::NOTIFICATION_RENDER_VIEW_HOST_CHANGED:
      if (!temporary_files_.empty()) {
        DeleteTemporaryFiles();

        // Now that the temporary files have been scheduled for deletion, there
        // is no longer any reason to keep this instance around.
        Release();
      }

      break;

    default:
      NOTREACHED();
  }
}

// static
bool FileSelectHelper::IsAcceptTypeValid(const std::string& accept_type) {
  // TODO(raymes): This only does some basic checks, extend to test more cases.
  // A 1 character accept type will always be invalid (either a "." in the case
  // of an extension or a "/" in the case of a MIME type).
  std::string unused;
  if (accept_type.length() <= 1 ||
      base::StringToLowerASCII(accept_type) != accept_type ||
      base::TrimWhitespaceASCII(accept_type, base::TRIM_ALL, &unused) !=
          base::TRIM_NONE) {
    return false;
  }
  return true;
}
