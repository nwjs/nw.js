// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// COPIED FROM web_view_browsertest.cc

#include <queue>
#include <set>
#include <utility>

#include "net/base/filename_util.h"
#include "base/test/test_timeouts.h"
#include "chrome/browser/media/webrtc/fake_desktop_media_picker_factory.h"

#include "content/public/browser/browser_task_traits.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/browser_test.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/guid.h"
#include "base/location.h"
#include "base/path_service.h"
#include "base/process/process.h"
#include "base/run_loop.h"
#include "base/task/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/apps/platform_apps/app_browsertest_util.h"
#include "chrome/browser/chrome_content_browser_client.h"
#include "chrome/browser/chrome_notification_types.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/download/download_history.h"
#include "chrome/browser/download/download_prefs.h"
#include "chrome/browser/download/download_core_service.h"
#include "chrome/browser/download/download_core_service_factory.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/pdf/pdf_extension_test_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/renderer_context_menu/render_view_context_menu.h"
#include "chrome/browser/renderer_context_menu/render_view_context_menu_test_util.h"
#include "chrome/browser/task_manager/task_manager_browsertest_util.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_dialogs.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/guest_view/browser/guest_view_manager.h"
#include "components/guest_view/browser/guest_view_manager_delegate.h"
#include "components/guest_view/browser/guest_view_manager_factory.h"
#include "components/guest_view/browser/test_guest_view_manager.h"
#include "content/public/browser/ax_event_notification_details.h"
#include "content/public/browser/browser_accessibility_state.h"
#include "content/public/browser/browser_child_process_host_iterator.h"
#include "content/public/browser/child_process_data.h"
#include "content/public/common/process_type.h"
#include "content/public/browser/gpu_data_manager.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/download_test_observer.h"
#include "content/public/test/test_renderer_host.h"
#include "extensions/browser/api/declarative/rules_registry.h"
#include "extensions/browser/api/declarative/rules_registry_service.h"
#include "extensions/browser/api/declarative/test_rules_registry.h"
#include "extensions/browser/api/declarative_webrequest/webrequest_constants.h"
#include "extensions/browser/api/extensions_api_client.h"
#include "extensions/browser/app_window/native_app_window.h"
#include "extensions/browser/guest_view/web_view/web_view_guest.h"
#include "extensions/common/extension.h"
#include "extensions/common/extensions_client.h"
#include "extensions/test/extension_test_message_listener.h"
#include "media/base/media_switches.h"
#include "net/test/embedded_test_server/embedded_test_server.h"
#include "net/test/embedded_test_server/http_request.h"
#include "net/test/embedded_test_server/http_response.h"
#include "ui/aura/env.h"
#include "ui/aura/window.h"
#include "ui/compositor/compositor.h"
#include "ui/compositor/compositor_observer.h"
#include "ui/display/display_switches.h"
#include "ui/events/event_switches.h"
#include "ui/events/gesture_detection/gesture_configuration.h"
#include "ui/gl/gl_switches.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget.h"

#include "chrome/browser/printing/print_preview_dialog_controller.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"

#include "chrome/browser/extensions/api/desktop_capture/desktop_capture_api.h"
#include "chrome/browser/media/webrtc/fake_desktop_media_list.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/test/base/ui_test_utils.h"
#include "content/public/test/browser_test_utils.h"
#include "extensions/common/switches.h"
#include "net/dns/mock_host_resolver.h"
#include "net/test/embedded_test_server/embedded_test_server.h"
#include "third_party/webrtc/modules/desktop_capture/desktop_capture_types.h"

#if defined(ENABLE_PLUGINS)
#include "content/public/browser/plugin_service.h"
#include "content/public/common/webplugininfo.h"
#include "content/public/test/ppapi_test_utils.h"
#endif

#include "third_party/blink/public/common/switches.h"

using extensions::ContextMenuMatcher;
using extensions::ExtensionsAPIClient;
using extensions::MenuItem;
using guest_view::GuestViewManager;
using guest_view::TestGuestViewManager;
using guest_view::TestGuestViewManagerFactory;
//using prerender::PrerenderLinkManager;
//using prerender::PrerenderLinkManagerFactory;
using task_manager::browsertest_util::MatchAboutBlankTab;
using task_manager::browsertest_util::MatchAnyApp;
using task_manager::browsertest_util::MatchAnyBackground;
using task_manager::browsertest_util::MatchAnyTab;
using task_manager::browsertest_util::MatchAnyWebView;
using task_manager::browsertest_util::MatchApp;
using task_manager::browsertest_util::MatchBackground;
using task_manager::browsertest_util::MatchWebView;
using task_manager::browsertest_util::WaitForTaskManagerRows;
using ui::MenuModel;
using content::BrowserThread;
using content::WebContents;

namespace {
const char kEmptyResponsePath[] = "/close-socket";
const char kRedirectResponsePath[] = "/server-redirect";
const char kUserAgentRedirectResponsePath[] = "/detect-user-agent";
const char kCacheResponsePath[] = "/cache-control-response";
const char kRedirectResponseFullPath[] =
    "/extensions/platform_apps/web_view/shim/guest_redirect.html";

class WebContentsHiddenObserver : public content::WebContentsObserver {
 public:
  WebContentsHiddenObserver(content::WebContents* web_contents,
                            const base::RepeatingClosure& hidden_callback)
      : WebContentsObserver(web_contents),
        hidden_callback_(hidden_callback),
        hidden_observed_(false) {
  }

  // WebContentsObserver.
  void OnVisibilityChanged(content::Visibility visibility) override {
    if (visibility == content::Visibility::HIDDEN) {
      hidden_observed_ = true;
      hidden_callback_.Run();
    }
  }

  bool hidden_observed() { return hidden_observed_; }

 private:
  base::RepeatingClosure hidden_callback_;
  bool hidden_observed_;

};

class EmbedderWebContentsObserver : public content::WebContentsObserver {
 public:
  explicit EmbedderWebContentsObserver(content::WebContents* web_contents)
      : WebContentsObserver(web_contents), terminated_(false) {}

  // WebContentsObserver.
  void PrimaryMainFrameRenderProcessGone(base::TerminationStatus status) override {
    terminated_ = true;
    if (message_loop_runner_.get())
      message_loop_runner_->Quit();
  }

  void WaitForEmbedderRenderProcessTerminate() {
    if (terminated_)
      return;
    message_loop_runner_ = new content::MessageLoopRunner;
    message_loop_runner_->Run();
  }

 private:
  bool terminated_;
  scoped_refptr<content::MessageLoopRunner> message_loop_runner_;

};

#if defined (USE_AURA)

// Waits for select control shown/closed.
class SelectControlWaiter : public aura::WindowObserver,
                            public aura::EnvObserver {
 public:
  SelectControlWaiter() {
    aura::Env::GetInstance()->AddObserver(this);
  }

  ~SelectControlWaiter() override {
    aura::Env::GetInstance()->RemoveObserver(this);
  }

  void Wait(bool wait_for_widget_shown) {
    wait_for_widget_shown_ = wait_for_widget_shown;
    message_loop_runner_ = new content::MessageLoopRunner;
    message_loop_runner_->Run();
    base::RunLoop().RunUntilIdle();
  }

  void OnWindowVisibilityChanged(aura::Window* window, bool visible) override {
    if (wait_for_widget_shown_ && visible)
      message_loop_runner_->Quit();
  }

  void OnWindowInitialized(aura::Window* window) override {
    if (window->GetType() != aura::client::WINDOW_TYPE_MENU)
      return;
    window->AddObserver(this);
    observed_windows_.insert(window);
  }

  void OnWindowDestroyed(aura::Window* window) override {
    observed_windows_.erase(window);
    if (!wait_for_widget_shown_ && observed_windows_.empty())
      message_loop_runner_->Quit();
  }

 private:
  scoped_refptr<content::MessageLoopRunner> message_loop_runner_;
  std::set<aura::Window*> observed_windows_;
  bool wait_for_widget_shown_ = false;

};

class LeftMouseClick {
 public:
  explicit LeftMouseClick(content::WebContents* web_contents)
      : web_contents_(web_contents),
        mouse_event_(blink::WebInputEvent::Type::kMouseDown,
                     blink::WebInputEvent::kNoModifiers,
                     blink::WebInputEvent::GetStaticTimeStampForTests()) {
    mouse_event_.button = blink::WebMouseEvent::Button::kLeft;
  }

  ~LeftMouseClick() {
    DCHECK(click_completed_);
  }

  void Click(const gfx::Point& point, int duration_ms) {
    DCHECK(click_completed_);
    click_completed_ = false;
    mouse_event_.SetType(blink::WebInputEvent::Type::kMouseDown);
    mouse_event_.SetPositionInWidget(point.x(), point.y());
    const gfx::Rect offset = web_contents_->GetContainerBounds();
    mouse_event_.SetPositionInScreen(point.x() + offset.x(),
                                     point.y() + offset.y());
    mouse_event_.click_count = 1;
    web_contents_->GetRenderViewHost()->GetWidget()->ForwardMouseEvent(
        mouse_event_);

    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
        FROM_HERE, base::BindOnce(&LeftMouseClick::SendMouseUp,
                              base::Unretained(this)),
        base::Milliseconds(duration_ms));
  }

  // Wait for click completed.
  void Wait() {
    if (click_completed_)
      return;
    message_loop_runner_ = new content::MessageLoopRunner;
    message_loop_runner_->Run();
    message_loop_runner_ = nullptr;
  }

 private:
  void SendMouseUp() {
    mouse_event_.SetType(blink::WebInputEvent::Type::kMouseUp);
    web_contents_->GetRenderViewHost()->GetWidget()->ForwardMouseEvent(
        mouse_event_);
    click_completed_ = true;
    if (message_loop_runner_)
      message_loop_runner_->Quit();
  }

  // Unowned pointer.
  content::WebContents* web_contents_;

  scoped_refptr<content::MessageLoopRunner> message_loop_runner_;

  blink::WebMouseEvent mouse_event_;

  bool click_completed_ = true;

};

#endif

}  // namespace

// This class intercepts media access request from the embedder. The request
// should be triggered only if the embedder API (from tests) allows the request
// in Javascript.
// We do not issue the actual media request; the fact that the request reached
// embedder's WebContents is good enough for our tests. This is also to make
// the test run successfully on trybots.
class MockWebContentsDelegate : public content::WebContentsDelegate {
 public:
  MockWebContentsDelegate()
      : requested_(false),
        checked_(false) {}
  ~MockWebContentsDelegate() override {}

  void RequestMediaAccessPermission(
      content::WebContents* web_contents,
      const content::MediaStreamRequest& request,
      content::MediaResponseCallback callback) override {
    requested_ = true;
    if (request_message_loop_runner_.get())
      request_message_loop_runner_->Quit();
  }

  bool CheckMediaAccessPermission(content::RenderFrameHost* render_frame_host,
                                  const GURL& security_origin,
                                  blink::mojom::MediaStreamType type) override {
    checked_ = true;
    if (check_message_loop_runner_.get())
      check_message_loop_runner_->Quit();
    return true;
  }

  void WaitForRequestMediaPermission() {
    if (requested_)
      return;
    request_message_loop_runner_ = new content::MessageLoopRunner;
    request_message_loop_runner_->Run();
  }

  void WaitForCheckMediaPermission() {
    if (checked_)
      return;
    check_message_loop_runner_ = new content::MessageLoopRunner;
    check_message_loop_runner_->Run();
  }

 private:
  bool requested_;
  bool checked_;
  scoped_refptr<content::MessageLoopRunner> request_message_loop_runner_;
  scoped_refptr<content::MessageLoopRunner> check_message_loop_runner_;

};

// This class intercepts download request from the guest.
class MockDownloadWebContentsDelegate : public content::WebContentsDelegate {
 public:
  explicit MockDownloadWebContentsDelegate(
      content::WebContentsDelegate* orig_delegate)
      : orig_delegate_(orig_delegate),
        waiting_for_decision_(false),
        expect_allow_(false),
        decision_made_(false),
        last_download_allowed_(false) {}
  ~MockDownloadWebContentsDelegate() override {}

  void CanDownload(const GURL& url,
                   const std::string& request_method,
                   base::OnceCallback<void(bool)> callback) override {
    orig_delegate_->CanDownload(
        url, request_method,
        base::BindOnce(&MockDownloadWebContentsDelegate::DownloadDecided,
                       base::Unretained(this), std::move(callback)));
  }

  void WaitForCanDownload(bool expect_allow) {
    EXPECT_FALSE(waiting_for_decision_);
    waiting_for_decision_ = true;

    if (decision_made_) {
      EXPECT_EQ(expect_allow, last_download_allowed_);
      return;
    }

    expect_allow_ = expect_allow;
    message_loop_runner_ = new content::MessageLoopRunner;
    message_loop_runner_->Run();
  }

  void DownloadDecided(base::OnceCallback<void(bool)> callback, bool allow) {
    EXPECT_FALSE(decision_made_);
    decision_made_ = true;

    if (waiting_for_decision_) {
      EXPECT_EQ(expect_allow_, allow);
      if (message_loop_runner_.get())
        message_loop_runner_->Quit();
      std::move(callback).Run(allow);
      return;
    }
    last_download_allowed_ = allow;
    std::move(callback).Run(allow);
  }

  void Reset() {
    waiting_for_decision_ = false;
    decision_made_ = false;
  }

 private:
  content::WebContentsDelegate* orig_delegate_;
  bool waiting_for_decision_;
  bool expect_allow_;
  bool decision_made_;
  bool last_download_allowed_;
  scoped_refptr<content::MessageLoopRunner> message_loop_runner_;

};

// TODO(wjmaclean): Fix this test class at some point so it can be re-enabled on
// the site isolation bots, and then look at re-enabling WebViewFocusTest when
// that happens.
// https://crbug.com/503751
class NWWebViewTestBase : public extensions::PlatformAppBrowserTest {
 protected:
  void SetUp() override {
    extensions::PlatformAppBrowserTest::SetUp();
  }

  void TearDown() override {
    extensions::PlatformAppBrowserTest::TearDown();
  }

  void SetUpOnMainThread() override {
    extensions::PlatformAppBrowserTest::SetUpOnMainThread();
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    command_line->AppendSwitch(switches::kUseFakeDeviceForMediaStream);
    command_line->AppendSwitchASCII(blink::switches::kJavaScriptFlags, "--expose-gc");

    extensions::PlatformAppBrowserTest::SetUpCommandLine(command_line);
    base::PathService::Get(base::DIR_SOURCE_ROOT, &test_data_dir_);
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("content"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("nw"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("test"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("data"));
  }

  // Handles |request| by serving a redirect response if the |User-Agent| is
  // foobar.
  static std::unique_ptr<net::test_server::HttpResponse>
  UserAgentResponseHandler(const std::string& path,
                           const GURL& redirect_target,
                           const net::test_server::HttpRequest& request) {
    if (!base::StartsWith(path, request.relative_url,
                          base::CompareCase::SENSITIVE))
      return std::unique_ptr<net::test_server::HttpResponse>();

    auto it = request.headers.find("User-Agent");
    EXPECT_TRUE(it != request.headers.end());
    if (!base::StartsWith("foobar", it->second,
                          base::CompareCase::SENSITIVE))
      return std::unique_ptr<net::test_server::HttpResponse>();

    std::unique_ptr<net::test_server::BasicHttpResponse> http_response(
        new net::test_server::BasicHttpResponse);
    http_response->set_code(net::HTTP_MOVED_PERMANENTLY);
    http_response->AddCustomHeader("Location", redirect_target.spec());
    return std::move(http_response);
  }

  // Handles |request| by serving a redirect response.
  static std::unique_ptr<net::test_server::HttpResponse>
  RedirectResponseHandler(const std::string& path,
                          const GURL& redirect_target,
                          const net::test_server::HttpRequest& request) {
    if (!base::StartsWith(path, request.relative_url,
                          base::CompareCase::SENSITIVE))
      return std::unique_ptr<net::test_server::HttpResponse>();

    std::unique_ptr<net::test_server::BasicHttpResponse> http_response(
        new net::test_server::BasicHttpResponse);
    http_response->set_code(net::HTTP_MOVED_PERMANENTLY);
    http_response->AddCustomHeader("Location", redirect_target.spec());
    return std::move(http_response);
  }

  // Handles |request| by serving an empty response.
  static std::unique_ptr<net::test_server::HttpResponse> EmptyResponseHandler(
      const std::string& path,
      const net::test_server::HttpRequest& request) {
    if (base::StartsWith(path, request.relative_url,
                         base::CompareCase::SENSITIVE))
      return std::unique_ptr<net::test_server::HttpResponse>(
          new net::test_server::RawHttpResponse("", ""));

    return std::unique_ptr<net::test_server::HttpResponse>();
  }

  // Handles |request| by serving cache-able response.
  static std::unique_ptr<net::test_server::HttpResponse>
  CacheControlResponseHandler(const std::string& path,
                              const net::test_server::HttpRequest& request) {
    if (!base::StartsWith(path, request.relative_url,
                          base::CompareCase::SENSITIVE))
      return std::unique_ptr<net::test_server::HttpResponse>();

    std::unique_ptr<net::test_server::BasicHttpResponse> http_response(
        new net::test_server::BasicHttpResponse);
    http_response->AddCustomHeader("Cache-control", "max-age=3600");
    http_response->set_content_type("text/plain");
    http_response->set_content("dummy text");
    return std::move(http_response);
  }

  // Shortcut to return the current MenuManager.
  extensions::MenuManager* menu_manager() {
    return extensions::MenuManager::Get(browser()->profile());
  }

  // This gets all the items that any extension has registered for possible
  // inclusion in context menus.
  MenuItem::List GetItems() {
    MenuItem::List result;
    std::set<MenuItem::ExtensionKey> extension_ids =
        menu_manager()->ExtensionIds();
    std::set<MenuItem::ExtensionKey>::iterator i;
    for (i = extension_ids.begin(); i != extension_ids.end(); ++i) {
      const MenuItem::OwnedList* list = menu_manager()->MenuItems(*i);
      for (const auto& item : *list)
        result.push_back(item.get());
    }
    return result;
  }

  enum TestServer {
    NEEDS_TEST_SERVER,
    NO_TEST_SERVER
  };

  void TestHelper(const std::string& test_name,
                  const std::string& app_location,
                  TestServer test_server) {
    // For serving guest pages.
    if (test_server == NEEDS_TEST_SERVER) {
      if (!StartEmbeddedTestServer()) {
        LOG(ERROR) << "FAILED TO START TEST SERVER.";
        return;
      }
      embedded_test_server()->RegisterRequestHandler(base::BindRepeating(
          &NWWebViewTestBase::RedirectResponseHandler, kRedirectResponsePath,
          embedded_test_server()->GetURL(kRedirectResponseFullPath)));

      embedded_test_server()->RegisterRequestHandler(base::BindRepeating(
          &NWWebViewTestBase::EmptyResponseHandler, kEmptyResponsePath));

      embedded_test_server()->RegisterRequestHandler(base::BindRepeating(
          &NWWebViewTestBase::UserAgentResponseHandler,
          kUserAgentRedirectResponsePath,
          embedded_test_server()->GetURL(kRedirectResponseFullPath)));

      embedded_test_server()->RegisterRequestHandler(base::BindRepeating(
          &NWWebViewTestBase::CacheControlResponseHandler, kCacheResponsePath));
    }

    LoadAndLaunchPlatformApp(app_location.c_str(), "Launched");

    // Flush any pending events to make sure we start with a clean slate.
    content::RunAllPendingInMessageLoop();

    content::WebContents* embedder_web_contents =
        GetFirstAppWindowWebContents();
    if (!embedder_web_contents) {
      LOG(ERROR) << "UNABLE TO FIND EMBEDDER WEB CONTENTS.";
      return;
    }

    ExtensionTestMessageListener done_listener("TEST_PASSED");
    done_listener.set_failure_message("TEST_FAILED");
    if (!content::ExecuteScript(
            embedder_web_contents,
            base::StringPrintf("runTest('%s')", test_name.c_str()))) {
      LOG(ERROR) << "UNABLE TO START TEST.";
      return;
    }
    ASSERT_TRUE(done_listener.WaitUntilSatisfied());
  }

  content::WebContents* LoadGuest(const std::string& guest_path,
                                  const std::string& app_path) {
    GURL::Replacements replace_host;
    replace_host.SetHostStr("localhost");

    GURL guest_url = embedded_test_server()->GetURL(guest_path);
    guest_url = guest_url.ReplaceComponents(replace_host);

    ui_test_utils::UrlLoadObserver guest_observer(
        guest_url, content::NotificationService::AllSources());

    LoadAndLaunchPlatformApp(app_path.c_str(), "guest-loaded");

    guest_observer.Wait();
    content::Source<content::NavigationController> source =
        guest_observer.source();
    EXPECT_TRUE(source->DeprecatedGetWebContents()->GetRenderViewHost()->GetProcess()->
        IsForGuestsOnly());

    content::WebContents* guest_web_contents = source->DeprecatedGetWebContents();
    return guest_web_contents;
  }

  // Runs media_access/allow tests.
  void MediaAccessAPIAllowTestHelper(const std::string& test_name);

  // Runs media_access/deny tests, each of them are run separately otherwise
  // they timeout (mostly on Windows).
  void MediaAccessAPIDenyTestHelper(const std::string& test_name) {
    ASSERT_TRUE(StartEmbeddedTestServer());  // For serving guest pages.
    LoadAndLaunchPlatformApp("web_view/media_access/deny", "loaded");

    content::WebContents* embedder_web_contents =
        GetFirstAppWindowWebContents();
    ASSERT_TRUE(embedder_web_contents);

    ExtensionTestMessageListener test_run_listener("PASSED");
    test_run_listener.set_failure_message("FAILED");
    EXPECT_TRUE(
        content::ExecuteScript(
            embedder_web_contents,
            base::StringPrintf("startDenyTest('%s')", test_name.c_str())));
    ASSERT_TRUE(test_run_listener.WaitUntilSatisfied());
  }

  // Loads an app with a <webview> in it, returns once a guest is created.
  void LoadAppWithGuest(const std::string& app_path) {
    ExtensionTestMessageListener launched_listener("WebViewTest.LAUNCHED");
    launched_listener.set_failure_message("WebViewTest.FAILURE");
    LoadAndLaunchPlatformApp(app_path.c_str(), &launched_listener);

    guest_view_ = GetGuestViewManager()->WaitForSingleGuestViewCreated();
  }

  void SendMessageToEmbedder(const std::string& message) {
    EXPECT_TRUE(
        content::ExecuteScript(
            GetEmbedderWebContents(),
            base::StringPrintf("onAppCommand('%s');", message.c_str())));
  }

  void SendMessageToGuestAndWait(const std::string& message,
                                 const std::string& wait_message) {
    std::unique_ptr<ExtensionTestMessageListener> listener;
    if (!wait_message.empty()) {
      listener.reset(new ExtensionTestMessageListener(wait_message));
    }

    EXPECT_TRUE(
        content::ExecuteScript(
            GetGuestWebContents(),
            base::StringPrintf("onAppCommand('%s');", message.c_str())));

    if (listener) {
      ASSERT_TRUE(listener->WaitUntilSatisfied());
    }
  }

  void OpenContextMenu(content::WebContents* web_contents) {
    blink::WebMouseEvent mouse_event(blink::WebInputEvent::Type::kMouseDown,
                                     blink::WebInputEvent::kNoModifiers,
                                     blink::WebInputEvent::GetStaticTimeStampForTests());
    mouse_event.button = blink::WebMouseEvent::Button::kRight;
    mouse_event.SetPositionInWidget(1, 1);
    web_contents->GetRenderViewHost()->GetWidget()->ForwardMouseEvent(
        mouse_event);
    mouse_event.SetType(blink::WebInputEvent::Type::kMouseUp);
    web_contents->GetRenderViewHost()->GetWidget()->ForwardMouseEvent(
        mouse_event);
  }

  content::WebContents* GetGuestWebContents() {
    return guest_view_->web_contents();
  }

  content::WebContents* GetEmbedderWebContents() {
    if (!embedder_web_contents_) {
      embedder_web_contents_ = GetFirstAppWindowWebContents();
    }
    return embedder_web_contents_;
  }

  TestGuestViewManager* GetGuestViewManager() {
    TestGuestViewManager* manager = static_cast<TestGuestViewManager*>(
        TestGuestViewManager::FromBrowserContext(browser()->profile()));
    // TestGuestViewManager::WaitForSingleGuestCreated may and will get called
    // before a guest is created.
    if (!manager) {
      manager = static_cast<TestGuestViewManager*>(
          GuestViewManager::CreateWithDelegate(
              browser()->profile(),
              ExtensionsAPIClient::Get()->CreateGuestViewManagerDelegate(
                  browser()->profile())));
    }
    return manager;
  }

  NWWebViewTestBase() : guest_view_(NULL), embedder_web_contents_(NULL) {
    GuestViewManager::set_factory_for_testing(&factory_);
  }

  ~NWWebViewTestBase() override {}

 protected:
  //content::FrameWatcher frame_watcher_;

 private:
  TestGuestViewManagerFactory factory_;
  // Note that these are only set if you launch app using LoadAppWithGuest().
  guest_view::GuestViewBase* guest_view_;
  content::WebContents* embedder_web_contents_;
};

class NWJSWebViewTest : public NWWebViewTestBase, public testing::WithParamInterface<bool> {
 public:
  NWJSWebViewTest() {}
  ~NWJSWebViewTest() override {}

  void SetUpCommandLine(base::CommandLine* command_line) override {
    NWWebViewTestBase::SetUpCommandLine(command_line);

    trusted_ = GetParam();
  }
protected:
  bool trusted_;
};

INSTANTIATE_TEST_SUITE_P(NWJSWebViewTests, NWJSWebViewTest, testing::Bool());

class NWJSWebViewTestF: public NWWebViewTestBase {};

class NWAppTest : public extensions::PlatformAppBrowserTest {
public:
  NWAppTest() {}
  ~NWAppTest() override {}
  void SetUpCommandLine(base::CommandLine* command_line) override {
    extensions::PlatformAppBrowserTest::SetUpCommandLine(command_line);
    base::PathService::Get(base::DIR_SOURCE_ROOT, &test_data_dir_);
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("content"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("nw"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("test"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("data"));
  }
};

class NWJSAppTest : public NWAppTest {};

void NWTimeoutCallback(const std::string& timeout_message) {
  base::RunLoop::QuitCurrentWhenIdleDeprecated();
}

IN_PROC_BROWSER_TEST_F(NWAppTest, LocalFlash) {
  std::string contents;
  base::FilePath test_dir = test_data_dir_.Append(FILE_PATH_LITERAL("platform_apps")).Append(FILE_PATH_LITERAL("local_flash"));
  base::FilePath tpl_path = test_dir.Append(FILE_PATH_LITERAL("index.tpl"));
  base::FilePath swf_path = test_dir.Append(FILE_PATH_LITERAL("test.swf"));
  base::FilePath index_html = test_dir.Append(FILE_PATH_LITERAL("index.html"));
  ASSERT_TRUE(base::ReadFileToString(tpl_path, &contents));
  GURL swf_url = net::FilePathToFileURL(swf_path);
  base::ReplaceSubstringsAfterOffset(&contents, 0, "<swf_path>", swf_url.spec());
  EXPECT_GT(base::WriteFile(index_html, contents.c_str(), contents.size()), 0);

  LoadAndLaunchPlatformApp("local_flash", "Launched");
  content::WebContents* web_contents = GetFirstAppWindowWebContents();
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    web_contents = BrowserList::GetInstance()->GetLastActive()->tab_strip_model()->GetActiveWebContents();
  }
  ASSERT_TRUE(web_contents);
  std::u16string expected_title(u"Loaded");
  content::TitleWatcher title_watcher(web_contents, expected_title);

  EXPECT_EQ(expected_title, title_watcher.WaitAndGetTitle());
}

namespace {
class PrintDialogWaiter {
public:
  PrintDialogWaiter(content::WebContents* web_contents)
    : web_contents_(web_contents) {}

  WebContents* Wait() {
    WebContents* ret;
    if ((ret = CheckDlg()))
      return ret;

    base::RepeatingTimer check_timer;
    check_timer.Start(
        FROM_HERE,
        base::Milliseconds(200),
        this,
        &PrintDialogWaiter::OnTimer);

    runner_ = new content::MessageLoopRunner;
    runner_->Run();
    return CheckDlg();
  }

 private:
  WebContents* CheckDlg() {
    printing::PrintPreviewDialogController* dialog_controller =
        printing::PrintPreviewDialogController::GetInstance();
    return dialog_controller->GetPrintPreviewForContents(web_contents_);
  }

  void OnTimer() {
    DCHECK(runner_.get());
    if (CheckDlg())
      runner_->Quit();
  }
  content::WebContents* web_contents_;
  scoped_refptr<content::MessageLoopRunner> runner_;

};

static std::string DumpPdfAccessibilityTree(const ui::AXTreeUpdate& ax_tree) {
  // Create a string representation of the tree starting with the embedded
  // object.
  std::string ax_tree_dump;
  std::map<int32_t, int> id_to_indentation;
  //bool found_embedded_object = false;
  for (auto& node : ax_tree.nodes) {
#if 0
    if (node.role == ui::AX_ROLE_EMBEDDED_OBJECT)
      found_embedded_object = true;
    if (!found_embedded_object)
      continue;
#endif
    int indent = id_to_indentation[node.id];
    ax_tree_dump += std::string(2 * indent, ' ');
    ax_tree_dump += ui::ToString(node.role);

    std::string name = node.GetStringAttribute(ax::mojom::StringAttribute::kName);
    base::ReplaceChars(name, "\r", "\\r", &name);
    base::ReplaceChars(name, "\n", "\\n", &name);
    if (!name.empty())
      ax_tree_dump += " '" + name + "'";
    ax_tree_dump += "\n";
    for (size_t j = 0; j < node.child_ids.size(); ++j)
      id_to_indentation[node.child_ids[j]] = indent + 1;
  }

  return ax_tree_dump;
}

void CountFrames(int* frame_count,
                 content::RenderFrameHost* frame) {
  ++(*frame_count);
}

void DumpAxTree(std::string* dump_output, content::RenderFrameHost* frame) {
  content::RenderFrameHostImpl* f = static_cast<content::RenderFrameHostImpl*>(frame);
  content::BrowserAccessibilityManager* manager = f->GetOrCreateBrowserAccessibilityManager();
  ui::AXTreeUpdate ax_tree = manager->SnapshotAXTreeForTesting();
  std::string ax_tree_dump = DumpPdfAccessibilityTree(ax_tree);
  *dump_output += ax_tree_dump;
}

} //namespace

IN_PROC_BROWSER_TEST_F(NWJSWebViewTestF, SilentPrintChangeFooter) {
  content::BrowserAccessibilityState::GetInstance()->EnableAccessibility();
  base::ScopedAllowBlockingForTesting allow_blocking;
  base::FilePath test_dir = test_data_dir_.Append(FILE_PATH_LITERAL("platform_apps")).Append(FILE_PATH_LITERAL("silent_print"));
  base::FilePath output_pdf = test_data_dir_.Append(FILE_PATH_LITERAL("output.pdf"));
  ASSERT_TRUE(base::SetCurrentDirectory(test_dir));
  ASSERT_TRUE(base::DeleteFile(output_pdf));
  LoadAndLaunchPlatformApp("silent_print", "Launched");
  content::WebContents* web_contents = GetFirstAppWindowWebContents();
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    web_contents = BrowserList::GetInstance()->GetLastActive()->tab_strip_model()->GetActiveWebContents();
  }
  ASSERT_TRUE(web_contents);
  ExtensionTestMessageListener listener("Loaded");
  ASSERT_TRUE(content::ExecuteScript(web_contents, "document.getElementById('testbtn').click()"));
  EXPECT_TRUE(listener.WaitUntilSatisfied()) << "'" << listener.message()
                                             << "' message was not receieved";

  std::vector<content::RenderFrameHost*> guest_rfh_list;
  unsigned long n_guests = 2;
  LOG(WARNING) << "WaitForNumGuestsCreated";
  GetGuestViewManager()->WaitForNumGuestsCreated(n_guests);
  GetGuestViewManager()->GetGuestRenderFrameHostList(&guest_rfh_list);
  ASSERT_EQ(n_guests, guest_rfh_list.size());

  bool load_success = pdf_extension_test_util::EnsurePDFHasLoaded(guest_rfh_list[0]);
  EXPECT_TRUE(load_success);
  WaitForAccessibilityTreeToContainNodeWithName(content::WebContents::FromRenderFrameHost(guest_rfh_list[0]), "hello world\r\n");
  std::string tree_dump;
  std::string* pstr = &tree_dump;
  // Make sure all the frames in the dialog has access to the PDF
  // plugin.
  content::WebContents::FromRenderFrameHost(guest_rfh_list[1])->GetPrimaryMainFrame()->
    ForEachRenderFrameHost([pstr] (content::RenderFrameHost* rfh) { DumpAxTree(pstr, rfh); });
  LOG(INFO) << "ax tree: " << tree_dump;
  EXPECT_TRUE(tree_dump.find("nwtestfooter") != std::string::npos);
}

IN_PROC_BROWSER_TEST_F(NWJSAppTest, PrintChangeFooter) {
  content::BrowserAccessibilityState::GetInstance()->EnableAccessibility();
  LoadAndLaunchPlatformApp("print_test", "Launched");
  content::WebContents* web_contents = GetFirstAppWindowWebContents();
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    web_contents = BrowserList::GetInstance()->GetLastActive()->tab_strip_model()->GetActiveWebContents();
  }
  ASSERT_TRUE(web_contents);
  ASSERT_TRUE(content::ExecuteScript(web_contents, "document.getElementById('testbtn').click()"));
  LOG(INFO) << "waiting for print dialog";
  WebContents* preview_dialog = PrintDialogWaiter(web_contents).Wait();
  LOG(INFO) << "done";
  const int kExpectedFrameCount = 3;
  int frame_count;
  do {
    base::RunLoop run_loop;
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
        FROM_HERE, run_loop.QuitClosure(), base::Seconds(1));
    run_loop.Run();

    frame_count = 0;
    int* pcnt = &frame_count;
    preview_dialog->GetPrimaryMainFrame()->ForEachRenderFrameHost(
       [pcnt] (content::RenderFrameHost* rfh) { CountFrames(pcnt, rfh); });
  } while (frame_count < kExpectedFrameCount);
  ASSERT_EQ(kExpectedFrameCount, frame_count);
  WaitForAccessibilityTreeToContainNodeWithName(preview_dialog, "hello world\r\n");
  std::string tree_dump;
  std::string* pstr = &tree_dump;
  // Make sure all the frames in the dialog has access to the PDF plugin.
  preview_dialog->GetPrimaryMainFrame()->
    ForEachRenderFrameHost([pstr] (content::RenderFrameHost* rfh) { DumpAxTree(pstr, rfh); });
  EXPECT_TRUE(tree_dump.find("nwtestfooter") != std::string::npos);
}

IN_PROC_BROWSER_TEST_P(NWJSWebViewTest, LocalPDF) {
  std::string contents;
  base::FilePath test_dir = test_data_dir_.Append(FILE_PATH_LITERAL("platform_apps")).Append(FILE_PATH_LITERAL("local_pdf"));
  base::FilePath tpl_path = test_dir.Append(FILE_PATH_LITERAL("index.tpl"));
  base::FilePath pdf_path = test_dir.Append(FILE_PATH_LITERAL("test.pdf"));
  base::FilePath index_html = test_dir.Append(FILE_PATH_LITERAL("index.html"));
  ASSERT_TRUE(base::ReadFileToString(tpl_path, &contents));
  GURL pdf_url = net::FilePathToFileURL(pdf_path);
  base::ReplaceSubstringsAfterOffset(&contents, 0, "<pdf_path>", pdf_url.spec());
  base::ReplaceSubstringsAfterOffset(&contents, 0, "_partition_", trusted_ ? "partition=\"trusted\"" : "");
  EXPECT_GT(base::WriteFile(index_html, contents.c_str(), contents.size()), 0);
  LoadAndLaunchPlatformApp("local_pdf", "Launched");

  // Flush any pending events to make sure we start with a clean slate.
  content::RunAllPendingInMessageLoop();
  content::WebContents* web_contents = GetFirstAppWindowWebContents();
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    web_contents = BrowserList::GetInstance()->GetLastActive()->tab_strip_model()->GetActiveWebContents();
  }
  ASSERT_TRUE(web_contents);
  std::vector<content::WebContents*> guest_web_contents_list;
  unsigned long n_guests = trusted_ ? 2 : 1;
  EXPECT_TRUE(content::ExecuteScript(web_contents, std::string("test(") + (trusted_ ? "true" : "false") + ")"));
  if (!trusted_) {
    base::CancelableOnceClosure timeout(
          base::BindOnce(&NWTimeoutCallback, "pdf load timed out."));
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
          FROM_HERE, timeout.callback(), TestTimeouts::action_timeout());
    ExtensionTestMessageListener pass_listener("PASSED");
    EXPECT_TRUE(pass_listener.WaitUntilSatisfied());
  } else
    GetGuestViewManager()->WaitForNumGuestsCreated(n_guests);

  std::vector<content::RenderFrameHost*> guest_rfh_list;
  GetGuestViewManager()->GetGuestRenderFrameHostList(&guest_rfh_list);
  ASSERT_EQ(n_guests, guest_rfh_list.size());

  if (trusted_) {
    bool load_success = pdf_extension_test_util::EnsurePDFHasLoaded(guest_rfh_list[0]);
    EXPECT_TRUE(load_success);
  }
}

using content::DesktopMediaID;

namespace {

struct TestFlags {
  bool expect_screens;
  bool expect_windows;
  bool expect_tabs;
  bool expect_audio;
  content::DesktopMediaID selected_source;
  bool cancelled;

  // Following flags are set by FakeDesktopMediaPicker when it's created and
  // deleted.
  bool picker_created;
  bool picker_deleted;
};

#if 0
class FakeDesktopMediaPicker : public DesktopMediaPicker {
 public:
  explicit FakeDesktopMediaPicker(TestFlags* expectation)
      : expectation_(expectation),
        weak_factory_(this) {
    expectation_->picker_created = true;
  }
  ~FakeDesktopMediaPicker() override { expectation_->picker_deleted = true; }

  // DesktopMediaPicker interface.
  void Show(const DesktopMediaPicker::Params& params,
            std::vector<std::unique_ptr<DesktopMediaList>> source_lists,
            const DoneCallback& done_callback) override {
    bool show_screens = false;
    bool show_windows = false;
    bool show_tabs = false;

    for (auto& source_list : source_lists) {
      switch (source_list->GetMediaListType()) {
        case DesktopMediaID::TYPE_NONE:
          break;
        case DesktopMediaID::TYPE_SCREEN:
          show_screens = true;
          break;
        case DesktopMediaID::TYPE_WINDOW:
          show_windows = true;
          break;
        case DesktopMediaID::TYPE_WEB_CONTENTS:
          show_tabs = true;
          break;
      }
    }
    EXPECT_EQ(expectation_->expect_screens, show_screens);
    EXPECT_EQ(expectation_->expect_windows, show_windows);
    EXPECT_EQ(expectation_->expect_tabs, show_tabs);
    EXPECT_EQ(expectation_->expect_audio, params.request_audio);
    EXPECT_EQ(params.modality, ui::ModalType::MODAL_TYPE_CHILD);

    if (!expectation_->cancelled) {
      // Post a task to call the callback asynchronously.
      base::ThreadTaskRunnerHandle::Get()->PostTask(
          FROM_HERE, base::BindOnce(&FakeDesktopMediaPicker::CallCallback,
                                    weak_factory_.GetWeakPtr(), done_callback));
    } else {
      // If we expect the dialog to be cancelled then store the callback to
      // retain reference to the callback handler.
      done_callback_ = done_callback;
    }
  }

 private:
  void CallCallback(DoneCallback done_callback) {
    done_callback.Run(expectation_->selected_source);
  }

  TestFlags* expectation_;
  DoneCallback done_callback_;

  base::WeakPtrFactory<FakeDesktopMediaPicker> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(FakeDesktopMediaPicker);
};

class FakeDesktopMediaPickerFactory :
    public extensions::DesktopCaptureChooseDesktopMediaFunction::PickerFactory {
 public:
  FakeDesktopMediaPickerFactory() {}
  ~FakeDesktopMediaPickerFactory() override {}

  void SetTestFlags(TestFlags* test_flags, int tests_count) {
    test_flags_ = test_flags;
    tests_count_ = tests_count;
    current_test_ = 0;
  }

  std::unique_ptr<DesktopMediaPicker> CreatePicker() override {
    EXPECT_LE(current_test_, tests_count_);
    if (current_test_ >= tests_count_)
      return std::unique_ptr<DesktopMediaPicker>();
    ++current_test_;
    return std::unique_ptr<DesktopMediaPicker>(
        new FakeDesktopMediaPicker(test_flags_ + current_test_ - 1));
  }

  std::unique_ptr<DesktopMediaList> CreateMediaList(
      DesktopMediaID::Type type) override {
    EXPECT_LE(current_test_, tests_count_);
    return std::unique_ptr<DesktopMediaList>(new FakeDesktopMediaList(type));
  }

 private:
  TestFlags* test_flags_;
  int tests_count_;
  int current_test_;

  DISALLOW_COPY_AND_ASSIGN(FakeDesktopMediaPickerFactory);
};

#endif

class NWJSDesktopCaptureApiTest : public NWAppTest {
 public:
  NWJSDesktopCaptureApiTest() {
    extensions::DesktopCaptureChooseDesktopMediaFunction::
        SetPickerFactoryForTests(&picker_factory_);
  }
  ~NWJSDesktopCaptureApiTest() override {
    extensions::DesktopCaptureChooseDesktopMediaFunction::
        SetPickerFactoryForTests(NULL);
  }

  void SetUpOnMainThread() override {
    NWAppTest::SetUpOnMainThread();
    host_resolver()->AddRule("*", "127.0.0.1");
  }

 protected:
  GURL GetURLForPath(const std::string& host, const std::string& path) {
    std::string port = base::NumberToString(embedded_test_server()->port());
    GURL::Replacements replacements;
    replacements.SetHostStr(host);
    replacements.SetPortStr(port);
    return embedded_test_server()->GetURL(path).ReplaceComponents(replacements);
  }

  FakeDesktopMediaPickerFactory picker_factory_;
};

}  // namespace

IN_PROC_BROWSER_TEST_F(NWJSDesktopCaptureApiTest, CrossDomain) {
  FakeDesktopMediaPickerFactory::TestFlags test_flags[] = {
       {true, false, false, false, false,
       content::DesktopMediaID(content::DesktopMediaID::TYPE_SCREEN,
                               content::DesktopMediaID::kNullId), false},
  };
  picker_factory_.SetTestFlags(test_flags, std::size(test_flags));
  base::FilePath test_dir = test_data_dir_.Append(FILE_PATH_LITERAL("platform_apps")).Append(FILE_PATH_LITERAL("6212-crossdomain-screen"));
  embedded_test_server()->ServeFilesFromDirectory(test_dir);
  ASSERT_TRUE(embedded_test_server()->Start());

  net::HostPortPair host_and_port = embedded_test_server()->host_port_pair();
  LoadAndLaunchPlatformApp("6212-crossdomain-screen", "Launched");
  content::WebContents* web_contents = GetFirstAppWindowWebContents();
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    web_contents = BrowserList::GetInstance()->GetLastActive()->tab_strip_model()->GetActiveWebContents();
  }
  ASSERT_TRUE(web_contents);
  ExtensionTestMessageListener listener("Loaded");
  ASSERT_TRUE(content::ExecuteScript(web_contents, "document.getElementById('frame0').src='" +
                                     embedded_test_server()->GetURL("/remote.html").spec() + "'"));
  EXPECT_TRUE(listener.WaitUntilSatisfied()) << "'" << listener.message()
                                             << "' message was not receieved";
  std::vector<content::RenderFrameHost*> frames;
  std::vector<content::RenderFrameHost*>* f = &frames;
  web_contents->ForEachRenderFrameHost(
       [f](content::RenderFrameHost* rfh) {
               f->push_back(rfh);
       });
  ASSERT_TRUE(frames.size() == 2);
  content::ExecuteScriptAndGetValue(frames[1], "document.getElementById('testbtn').click()");
  ExtensionTestMessageListener pass_listener("Pass");
  EXPECT_TRUE(pass_listener.WaitUntilSatisfied()) << "'" << pass_listener.message()
                                             << "' message was not receieved";
}
