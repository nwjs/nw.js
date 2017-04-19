// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// COPIED FROM web_view_browsertest.cc

#include <queue>
#include <set>
#include <utility>

#include "net/base/filename_util.h"
#include "base/test/test_timeouts.h"

#include "base/callback_helpers.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/guid.h"
#include "base/location.h"
#include "base/macros.h"
#include "base/path_service.h"
#include "base/process/process.h"
#include "base/run_loop.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_task_runner_handle.h"
#include "build/build_config.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/apps/app_browsertest_util.h"
#include "chrome/browser/chrome_content_browser_client.h"
#include "chrome/browser/chrome_notification_types.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/download/download_history.h"
#include "chrome/browser/download/download_prefs.h"
#include "chrome/browser/download/download_service.h"
#include "chrome/browser/download/download_service_factory.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/pdf/pdf_extension_test_util.h"
#include "chrome/browser/prerender/prerender_link_manager.h"
#include "chrome/browser/prerender/prerender_link_manager_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/renderer_context_menu/render_view_context_menu.h"
#include "chrome/browser/renderer_context_menu/render_view_context_menu_test_util.h"
#include "chrome/browser/task_manager/task_manager_browsertest_util.h"
#include "chrome/browser/ui/browser.h"
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
#include "content/public/browser/interstitial_page.h"
#include "content/public/browser/interstitial_page_delegate.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/common/child_process_host.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/download_test_observer.h"
#include "content/public/test/fake_speech_recognition_manager.h"
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
#include "content/browser/frame_host/render_frame_host_impl.h"

#if defined(ENABLE_PLUGINS)
#include "content/public/browser/plugin_service.h"
#include "content/public/common/webplugininfo.h"
#include "content/public/test/ppapi_test_utils.h"
#endif

using extensions::ContextMenuMatcher;
using extensions::ExtensionsAPIClient;
using extensions::MenuItem;
using guest_view::GuestViewManager;
using guest_view::TestGuestViewManager;
using guest_view::TestGuestViewManagerFactory;
using prerender::PrerenderLinkManager;
using prerender::PrerenderLinkManagerFactory;
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

class TestInterstitialPageDelegate : public content::InterstitialPageDelegate {
 public:
  TestInterstitialPageDelegate() {
  }
  ~TestInterstitialPageDelegate() override {}
  std::string GetHTMLContents() override { return std::string(); }
};

class WebContentsHiddenObserver : public content::WebContentsObserver {
 public:
  WebContentsHiddenObserver(content::WebContents* web_contents,
                            const base::Closure& hidden_callback)
      : WebContentsObserver(web_contents),
        hidden_callback_(hidden_callback),
        hidden_observed_(false) {
  }

  // WebContentsObserver.
  void WasHidden() override {
    hidden_observed_ = true;
    hidden_callback_.Run();
  }

  bool hidden_observed() { return hidden_observed_; }

 private:
  base::Closure hidden_callback_;
  bool hidden_observed_;

  DISALLOW_COPY_AND_ASSIGN(WebContentsHiddenObserver);
};

// Watches for context menu to be shown, records count of how many times
// context menu was shown.
class ContextMenuCallCountObserver {
 public:
  ContextMenuCallCountObserver()
      : num_times_shown_(0),
        menu_observer_(chrome::NOTIFICATION_RENDER_VIEW_CONTEXT_MENU_SHOWN,
                       base::Bind(&ContextMenuCallCountObserver::OnMenuShown,
                                  base::Unretained(this))) {
  }
  ~ContextMenuCallCountObserver() {}

  bool OnMenuShown(const content::NotificationSource& source,
                   const content::NotificationDetails& details) {
    ++num_times_shown_;
    auto* context_menu = content::Source<RenderViewContextMenu>(source).ptr();
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::Bind(&RenderViewContextMenuBase::Cancel,
                              base::Unretained(context_menu)));
    return true;
  }

  void Wait() { menu_observer_.Wait(); }

  int num_times_shown() { return num_times_shown_; }

 private:
  int num_times_shown_;
  content::WindowedNotificationObserver menu_observer_;

  DISALLOW_COPY_AND_ASSIGN(ContextMenuCallCountObserver);
};

class EmbedderWebContentsObserver : public content::WebContentsObserver {
 public:
  explicit EmbedderWebContentsObserver(content::WebContents* web_contents)
      : WebContentsObserver(web_contents), terminated_(false) {}

  // WebContentsObserver.
  void RenderProcessGone(base::TerminationStatus status) override {
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

  DISALLOW_COPY_AND_ASSIGN(EmbedderWebContentsObserver);
};

#if defined (USE_AURA)

// Waits for select control shown/closed.
class SelectControlWaiter : public aura::WindowObserver,
                            public aura::EnvObserver {
 public:
  SelectControlWaiter() {
    aura::Env::GetInstanceDontCreate()->AddObserver(this);
  }

  ~SelectControlWaiter() override {
    aura::Env::GetInstanceDontCreate()->RemoveObserver(this);
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
    if (window->type() != ui::wm::WINDOW_TYPE_MENU)
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

  DISALLOW_COPY_AND_ASSIGN(SelectControlWaiter);
};

class LeftMouseClick {
 public:
  explicit LeftMouseClick(content::WebContents* web_contents)
      : web_contents_(web_contents),
        mouse_event_(blink::WebInputEvent::kMouseDown,
                     blink::WebInputEvent::kNoModifiers,
                     blink::WebInputEvent::kTimeStampForTesting) {
    mouse_event_.button = blink::WebMouseEvent::Button::kLeft;
  }

  ~LeftMouseClick() {
    DCHECK(click_completed_);
  }

  void Click(const gfx::Point& point, int duration_ms) {
    DCHECK(click_completed_);
    click_completed_ = false;
    mouse_event_.SetType(blink::WebInputEvent::kMouseDown);
    mouse_event_.SetPositionInWidget(point.x(), point.y());
    const gfx::Rect offset = web_contents_->GetContainerBounds();
    mouse_event_.SetPositionInScreen(point.x() + offset.x(),
                                     point.y() + offset.y());
    mouse_event_.click_count = 1;
    web_contents_->GetRenderViewHost()->GetWidget()->ForwardMouseEvent(
        mouse_event_);

    base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE, base::Bind(&LeftMouseClick::SendMouseUp,
                              base::Unretained(this)),
        base::TimeDelta::FromMilliseconds(duration_ms));
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
    mouse_event_.SetType(blink::WebInputEvent::kMouseUp);
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

  DISALLOW_COPY_AND_ASSIGN(LeftMouseClick);
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
      const content::MediaResponseCallback& callback) override {
    requested_ = true;
    if (request_message_loop_runner_.get())
      request_message_loop_runner_->Quit();
  }

  bool CheckMediaAccessPermission(content::WebContents* web_contents,
                                  const GURL& security_origin,
                                  content::MediaStreamType type) override {
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

  DISALLOW_COPY_AND_ASSIGN(MockWebContentsDelegate);
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
                   const base::Callback<void(bool)>& callback) override {
    orig_delegate_->CanDownload(
        url, request_method,
        base::Bind(&MockDownloadWebContentsDelegate::DownloadDecided,
                   base::Unretained(this), callback));
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

  void DownloadDecided(const base::Callback<void(bool)>& callback, bool allow) {
    EXPECT_FALSE(decision_made_);
    decision_made_ = true;

    if (waiting_for_decision_) {
      EXPECT_EQ(expect_allow_, allow);
      if (message_loop_runner_.get())
        message_loop_runner_->Quit();
      callback.Run(allow);
      return;
    }
    last_download_allowed_ = allow;
    callback.Run(allow);
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

  DISALLOW_COPY_AND_ASSIGN(MockDownloadWebContentsDelegate);
};

// TODO(wjmaclean): Fix this test class at some point so it can be re-enabled on
// the site isolation bots, and then look at re-enabling WebViewFocusTest when
// that happens.
// https://crbug.com/503751
class NWWebViewTestBase : public extensions::PlatformAppBrowserTest {
 protected:
  void SetUp() override {
    if (UsesFakeSpeech()) {
      // SpeechRecognition test specific SetUp.
      fake_speech_recognition_manager_.reset(
          new content::FakeSpeechRecognitionManager());
      fake_speech_recognition_manager_->set_should_send_fake_response(true);
      // Inject the fake manager factory so that the test result is returned to
      // the web page.
      content::SpeechRecognitionManager::SetManagerForTesting(
          fake_speech_recognition_manager_.get());
    }
    extensions::PlatformAppBrowserTest::SetUp();
  }

  void TearDown() override {
    if (UsesFakeSpeech()) {
      // SpeechRecognition test specific TearDown.
      content::SpeechRecognitionManager::SetManagerForTesting(NULL);
    }

    extensions::PlatformAppBrowserTest::TearDown();
  }

  void SetUpOnMainThread() override {
    extensions::PlatformAppBrowserTest::SetUpOnMainThread();
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();
    // Mock out geolocation for geolocation specific tests.
    if (!strncmp(test_info->name(), "GeolocationAPI",
            strlen("GeolocationAPI"))) {
      ui_test_utils::OverrideGeolocation(10, 20);
    }
  }

  void SetUpCommandLine(base::CommandLine* command_line) override {
    command_line->AppendSwitch(switches::kUseFakeDeviceForMediaStream);
    command_line->AppendSwitchASCII(switches::kJavaScriptFlags, "--expose-gc");

    extensions::PlatformAppBrowserTest::SetUpCommandLine(command_line);
    PathService::Get(base::DIR_SOURCE_ROOT, &test_data_dir_);
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
      embedded_test_server()->RegisterRequestHandler(base::Bind(
          &NWWebViewTestBase::RedirectResponseHandler, kRedirectResponsePath,
          embedded_test_server()->GetURL(kRedirectResponseFullPath)));

      embedded_test_server()->RegisterRequestHandler(base::Bind(
          &NWWebViewTestBase::EmptyResponseHandler, kEmptyResponsePath));

      embedded_test_server()->RegisterRequestHandler(base::Bind(
          &NWWebViewTestBase::UserAgentResponseHandler,
          kUserAgentRedirectResponsePath,
          embedded_test_server()->GetURL(kRedirectResponseFullPath)));

      embedded_test_server()->RegisterRequestHandler(base::Bind(
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

    ExtensionTestMessageListener done_listener("TEST_PASSED", false);
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
    EXPECT_TRUE(source->GetWebContents()->GetRenderProcessHost()->
        IsForGuestsOnly());

    content::WebContents* guest_web_contents = source->GetWebContents();
    return guest_web_contents;
  }

  // Helper to load interstitial page in a <webview>.
  void InterstitialTeardownTestHelper() {
    // Start a HTTPS server so we can load an interstitial page inside guest.
    net::EmbeddedTestServer https_server(net::EmbeddedTestServer::TYPE_HTTPS);
    https_server.SetSSLConfig(net::EmbeddedTestServer::CERT_MISMATCHED_NAME);
    https_server.ServeFilesFromSourceDirectory("chrome/test/data");
    ASSERT_TRUE(https_server.Start());

    net::HostPortPair host_and_port = https_server.host_port_pair();

    LoadAndLaunchPlatformApp("web_view/interstitial_teardown",
                             "EmbedderLoaded");

    // Now load the guest.
    content::WebContents* embedder_web_contents =
        GetFirstAppWindowWebContents();
    ExtensionTestMessageListener second("GuestAddedToDom", false);
    EXPECT_TRUE(content::ExecuteScript(
        embedder_web_contents,
        base::StringPrintf("loadGuest(%d);\n", host_and_port.port())));
    ASSERT_TRUE(second.WaitUntilSatisfied());

    // Wait for interstitial page to be shown in guest.
    content::WebContents* guest_web_contents =
        GetGuestViewManager()->WaitForSingleGuestCreated();
    ASSERT_TRUE(guest_web_contents->GetRenderProcessHost()->IsForGuestsOnly());
    content::WaitForInterstitialAttach(guest_web_contents);
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

    ExtensionTestMessageListener test_run_listener("PASSED", false);
    test_run_listener.set_failure_message("FAILED");
    EXPECT_TRUE(
        content::ExecuteScript(
            embedder_web_contents,
            base::StringPrintf("startDenyTest('%s')", test_name.c_str())));
    ASSERT_TRUE(test_run_listener.WaitUntilSatisfied());
  }

  // Loads an app with a <webview> in it, returns once a guest is created.
  void LoadAppWithGuest(const std::string& app_path) {
    ExtensionTestMessageListener launched_listener("WebViewTest.LAUNCHED",
                                                   false);
    launched_listener.set_failure_message("WebViewTest.FAILURE");
    LoadAndLaunchPlatformApp(app_path.c_str(), &launched_listener);

    guest_web_contents_ = GetGuestViewManager()->WaitForSingleGuestCreated();
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
      listener.reset(new ExtensionTestMessageListener(wait_message, false));
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
    blink::WebMouseEvent mouse_event(blink::WebInputEvent::kMouseDown,
                                     blink::WebInputEvent::kNoModifiers,
                                     blink::WebInputEvent::kTimeStampForTesting);
    mouse_event.button = blink::WebMouseEvent::Button::kRight;
    mouse_event.SetPositionInWidget(1, 1);
    web_contents->GetRenderViewHost()->GetWidget()->ForwardMouseEvent(
        mouse_event);
    mouse_event.SetType(blink::WebInputEvent::kMouseUp);
    web_contents->GetRenderViewHost()->GetWidget()->ForwardMouseEvent(
        mouse_event);
  }

  content::WebContents* GetGuestWebContents() {
    return guest_web_contents_;
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

  NWWebViewTestBase() : guest_web_contents_(NULL), embedder_web_contents_(NULL) {
    GuestViewManager::set_factory_for_testing(&factory_);
  }

  ~NWWebViewTestBase() override {}

 protected:
  content::FrameWatcher frame_watcher_;

 private:
  bool UsesFakeSpeech() {
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    // SpeechRecognition test specific SetUp.
    const char* name = "SpeechRecognitionAPI_HasPermissionAllow";
    return !strncmp(test_info->name(), name, strlen(name));
  }

  std::unique_ptr<content::FakeSpeechRecognitionManager>
      fake_speech_recognition_manager_;

  TestGuestViewManagerFactory factory_;
  // Note that these are only set if you launch app using LoadAppWithGuest().
  content::WebContents* guest_web_contents_;
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

INSTANTIATE_TEST_CASE_P(NWJSWebViewTests, NWJSWebViewTest, testing::Bool());

class NWJSWebViewTestF: public NWWebViewTestBase {};

class NWAppTest : public extensions::PlatformAppBrowserTest {
public:
  NWAppTest() {}
  ~NWAppTest() override {}
  void SetUpCommandLine(base::CommandLine* command_line) override {
    extensions::PlatformAppBrowserTest::SetUpCommandLine(command_line);
    PathService::Get(base::DIR_SOURCE_ROOT, &test_data_dir_);
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("content"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("nw"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("test"));
    test_data_dir_ = test_data_dir_.Append(FILE_PATH_LITERAL("data"));
  }
  static void CountPluginProcesses(int* count, const base::Closure& quit_task) {
    for (content::BrowserChildProcessHostIterator iter; !iter.Done(); ++iter) {
      if (iter.GetData().process_type == content::PROCESS_TYPE_PPAPI_PLUGIN)
        (*count)++;
    }
    BrowserThread::PostTask(BrowserThread::UI, FROM_HERE, quit_task);
  }
  static void EnsureFlashProcessCount(int expected) {
    int actual = 0;
    scoped_refptr<content::MessageLoopRunner> runner =
        new content::MessageLoopRunner;
    BrowserThread::PostTask(
        BrowserThread::IO,
        FROM_HERE,
        base::Bind(&CountPluginProcesses, &actual, runner->QuitClosure()));
    runner->Run();
    ASSERT_EQ(expected, actual);
  }

};

class NWJSAppTest : public NWAppTest {};

void NWTimeoutCallback(const std::string& timeout_message) {
  base::MessageLoop::current()->QuitWhenIdle();
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
  ASSERT_TRUE(web_contents);
  base::string16 expected_title(base::ASCIIToUTF16("Loaded"));
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
        base::TimeDelta::FromMilliseconds(200),
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

  DISALLOW_COPY_AND_ASSIGN(PrintDialogWaiter);
};

static std::string DumpPdfAccessibilityTree(const ui::AXTreeUpdate& ax_tree) {
  // Create a string representation of the tree starting with the embedded
  // object.
  std::string ax_tree_dump;
  base::hash_map<int32_t, int> id_to_indentation;
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

    std::string name = node.GetStringAttribute(ui::AX_ATTR_NAME);
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
  base::FilePath test_dir = test_data_dir_.Append(FILE_PATH_LITERAL("platform_apps")).Append(FILE_PATH_LITERAL("silent_print"));
  base::FilePath output_pdf = test_data_dir_.Append(FILE_PATH_LITERAL("output.pdf"));
  ASSERT_TRUE(base::SetCurrentDirectory(test_dir));
  ASSERT_TRUE(base::DeleteFile(output_pdf, false));
  LoadAndLaunchPlatformApp("silent_print", "Launched");
  content::WebContents* web_contents = GetFirstAppWindowWebContents();
  ASSERT_TRUE(web_contents);
  ExtensionTestMessageListener listener("Loaded", false);
  ASSERT_TRUE(content::ExecuteScript(web_contents, "document.getElementById('testbtn').click()"));
  EXPECT_TRUE(listener.WaitUntilSatisfied()) << "'" << listener.message()
                                             << "' message was not receieved";

  std::vector<content::WebContents*> guest_web_contents_list;
  unsigned long n_guests = 2;
  LOG(WARNING) << "WaitForNumGuestsCreated";
  GetGuestViewManager()->WaitForNumGuestsCreated(n_guests);
  GetGuestViewManager()->GetGuestWebContentsList(&guest_web_contents_list);
  ASSERT_EQ(n_guests, guest_web_contents_list.size());

  content::WebContents* web_view_contents = guest_web_contents_list[0];
  bool load_success = pdf_extension_test_util::EnsurePDFHasLoaded(
      web_view_contents);
  EXPECT_TRUE(load_success);
  WaitForAccessibilityTreeToContainNodeWithName(web_view_contents, "hello world\r\n");
  std::string tree_dump;
  // Make sure all the frames in the dialog has access to the PDF plugin.
  guest_web_contents_list[1]->ForEachFrame(base::Bind(&DumpAxTree, base::Unretained(&tree_dump)));
  LOG(INFO) << "ax tree: " << tree_dump;
  EXPECT_TRUE(tree_dump.find("nwtestfooter") != std::string::npos);
}

IN_PROC_BROWSER_TEST_F(NWJSAppTest, PrintChangeFooter) {
  content::BrowserAccessibilityState::GetInstance()->EnableAccessibility();
  LoadAndLaunchPlatformApp("print_test", "Launched");
  content::WebContents* web_contents = GetFirstAppWindowWebContents();
  ASSERT_TRUE(web_contents);
  ASSERT_TRUE(content::ExecuteScript(web_contents, "document.getElementById('testbtn').click()"));
  LOG(INFO) << "waiting for print dialog";
  WebContents* preview_dialog = PrintDialogWaiter(web_contents).Wait();
  LOG(INFO) << "done";
  const int kExpectedFrameCount = 2;
  int frame_count;
  do {
    base::RunLoop run_loop;
    base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE, run_loop.QuitClosure(), base::TimeDelta::FromSeconds(1));
    run_loop.Run();

    frame_count = 0;
    preview_dialog->ForEachFrame(
        base::Bind(&CountFrames, base::Unretained(&frame_count)));
  } while (frame_count < kExpectedFrameCount);
  ASSERT_EQ(kExpectedFrameCount, frame_count);
  WaitForAccessibilityTreeToContainNodeWithName(preview_dialog, "hello world\r\n");
  std::string tree_dump;
  // Make sure all the frames in the dialog has access to the PDF plugin.
  preview_dialog->ForEachFrame(base::Bind(&DumpAxTree, base::Unretained(&tree_dump)));
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

  std::vector<content::WebContents*> guest_web_contents_list;
  unsigned long n_guests = trusted_ ? 2 : 1;
  LOG(WARNING) << "WaitForNumGuestsCreated";
  if (!trusted_) {
    base::CancelableClosure timeout(
          base::Bind(&NWTimeoutCallback, "pdf load timed out."));
    base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
          FROM_HERE, timeout.callback(), TestTimeouts::action_timeout());
    
    GetGuestViewManager()->WaitForNumGuestsCreated(2u);
  } else
    GetGuestViewManager()->WaitForNumGuestsCreated(n_guests);

  GetGuestViewManager()->GetGuestWebContentsList(&guest_web_contents_list);
  ASSERT_EQ(n_guests, guest_web_contents_list.size());

  if (trusted_) {
    content::WebContents* web_view_contents = guest_web_contents_list[0];
    bool load_success = pdf_extension_test_util::EnsurePDFHasLoaded(
      web_view_contents);
    EXPECT_TRUE(load_success);
  }
}

