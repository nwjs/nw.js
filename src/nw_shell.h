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

#ifndef CONTENT_NW_SRC_SHELL_H_
#define CONTENT_NW_SRC_SHELL_H_

#include <vector>

#include "base/basictypes.h"
#include "base/memory/weak_ptr.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "ipc/ipc_channel.h"

namespace base {
class DictionaryValue;
class FilePath;
}

namespace extensions {
struct DraggableRegion;
}

class GURL;

namespace nw {
class NativeWindow;
class Package;
}

namespace content {

class BrowserContext;
class ShellDevToolsFrontend;
class ShellJavaScriptDialogCreator;
class SiteInstance;
class WebContents;

using base::FilePath;

// This represents one window of the Content Shell, i.e. all the UI including
// buttons and url bar, as well as the web content area.
class Shell : public WebContentsDelegate,
              public content::WebContentsObserver,
              public NotificationObserver {
 public:
  enum ReloadType {
    RELOAD,                      // Normal (cache-validating) reload.
    RELOAD_IGNORING_CACHE,       // Reload bypassing the cache (shift-reload).
    RELOAD_ORIGINAL_REQUEST_URL, // Reload using the original request
                                 // URL.
    RELOAD_DEV                   // Refresh code from disk, should start a new renderer
                                 // process
  };

  explicit Shell(WebContents* web_contents, base::DictionaryValue* manifest);
  virtual ~Shell();

  // Create a new shell.
  static Shell* Create(BrowserContext* browser_context,
                       const GURL& url,
                       SiteInstance* site_instance,
                       int routing_id,
                       WebContents* base_web_contents);

  // Create a new shell for window.open and Window.open
  static Shell* Create(WebContents* source_contents,
                       const GURL& target_url,
                       base::DictionaryValue* manifest,
                       WebContents* new_contents);

  // Returns the Shell object corresponding to the given RenderViewHost.
  static Shell* FromRenderViewHost(RenderViewHost* rvh);

  void LoadURL(const GURL& url);
  void GoBackOrForward(int offset);
  void Reload(ReloadType type = RELOAD);
  void Stop();
  void ReloadOrStop();
  void ShowDevTools();
#if 0
  void CloseDevTools();
#endif
  // Send an event to renderer.
  void SendEvent(const std::string& event, const std::string& arg1 = "");

  // Decide whether we should close the window.
  bool ShouldCloseWindow();

  // Print critical error.
  void PrintCriticalError(const std::string& title,
                          const std::string& content);

  // Returns the currently open windows.
  static std::vector<Shell*>& windows() { return windows_; }

  static nw::Package* GetPackage();

  static void set_exit_code(int code) { exit_code_ = code; }

  static int exit_code() { return exit_code_; }

  WebContents* web_contents() const { return web_contents_.get(); }
  nw::NativeWindow* window() { return window_.get(); }

  void set_force_close(bool force) { force_close_ = force; }
  bool is_devtools() const { return is_devtools_; }
  bool nodejs() const { return enable_nodejs_; }
  bool force_close() const { return force_close_; }
  void set_id(int id) { id_ = id; }
  int id() const { return id_; }

 protected:
  // content::WebContentsObserver implementation.
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

  // content::WebContentsDelegate implementation.
  virtual WebContents* OpenURLFromTab(WebContents* source,
                                      const OpenURLParams& params) OVERRIDE;
  virtual void LoadingStateChanged(WebContents* source) OVERRIDE;
  virtual void ActivateContents(content::WebContents* contents) OVERRIDE;
  virtual void DeactivateContents(content::WebContents* contents) OVERRIDE;
  virtual void CloseContents(WebContents* source) OVERRIDE;
  virtual void MoveContents(WebContents* source, const gfx::Rect& pos) OVERRIDE;
  virtual bool IsPopupOrPanel(const WebContents* source) const OVERRIDE;
  virtual void WebContentsCreated(WebContents* source_contents,
                                  int64 source_frame_id,
                                  const GURL& target_url,
                                  WebContents* new_contents) OVERRIDE;
  virtual void RunFileChooser(
      content::WebContents* web_contents,
      const content::FileChooserParams& params) OVERRIDE;
  virtual void EnumerateDirectory(content::WebContents* web_contents,
                                  int request_id,
                                  const FilePath& path) OVERRIDE;
  virtual void DidNavigateMainFramePostCommit(
      WebContents* web_contents) OVERRIDE;
  virtual JavaScriptDialogManager* GetJavaScriptDialogManager() OVERRIDE;
  virtual void RequestToLockMouse(WebContents* web_contents,
                                  bool user_gesture,
                                  bool last_unlocked_by_target) OVERRIDE;
  virtual void HandleKeyboardEvent(
      WebContents* source,
      const NativeWebKeyboardEvent& event) OVERRIDE;
  virtual bool AddMessageToConsole(WebContents* source,
                                   int32 level,
                                   const string16& message,
                                   int32 line_no,
                                   const string16& source_id) OVERRIDE;
  virtual void RequestMediaAccessPermission(
      WebContents* web_contents,
      const MediaStreamRequest& request,
      const MediaResponseCallback& callback) OVERRIDE;

 private:
  void UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions);

  // NotificationObserver
  virtual void Observe(int type,
                       const NotificationSource& source,
                       const NotificationDetails& details) OVERRIDE;

  scoped_ptr<ShellJavaScriptDialogCreator> dialog_creator_;
  scoped_ptr<WebContents> web_contents_;
  scoped_ptr<nw::NativeWindow> window_;

  // Notification manager.
  NotificationRegistrar registrar_;

  // Weak potiner to devtools window.
  base::WeakPtr<Shell> devtools_window_;
#if 0
  ShellDevToolsFrontend* devtools_frontend_;
#endif
  // Factory to generate weak pointer, used by devtools.
  base::WeakPtrFactory<Shell> weak_ptr_factory_;

  // Whether this shell is devtools window.
  bool is_devtools_;

  // Flag to indicate we will force closing.
  bool force_close_;

  // ID of corresponding js object.
  int id_;

  bool enable_nodejs_;
  // A container of all the open windows. We use a vector so we can keep track
  // of ordering.
  static std::vector<Shell*> windows_;

  // True if the destructur of Shell should post a quit closure on the current
  // message loop if the destructed Shell object was the last one.
  static bool quit_message_loop_;

  static int exit_code_;
};

}  // namespace content

#endif  // CONTENT_NW_SRC_SHELL_H_
