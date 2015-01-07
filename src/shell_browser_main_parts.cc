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

#include "content/nw/src/shell_browser_main_parts.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "chrome/common/chrome_switches.h"
#include "content/nw/src/api/app/app.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/breakpad_linux.h"
#include "chrome/browser/printing/print_job_manager.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/browser/nw_constrained_window_views_client.h"
#include "content/nw/src/browser/shell_devtools_manager_delegate.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/main_function_params.h"
#include "net/grit/net_resources.h"
#include "net/base/net_module.h"
#include "net/proxy/proxy_resolver_v8.h"
#include "ui/base/ime/input_method_initializer.h"
#include "ui/base/resource/resource_bundle.h"

#if !defined(OS_WIN)
#include <sys/resource.h>
#endif

#if defined(TOOLKIT_GTK)
#include "content/nw/src/browser/printing/print_dialog_gtk.h"
#endif

#if defined(USE_AURA)
#include "ui/aura/env.h"
#include "ui/gfx/screen.h"
#include "ui/views/test/desktop_test_views_delegate.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#endif  // defined(USE_AURA)

#if defined(OS_LINUX)
#include "chrome/browser/ui/libgtk2ui/gtk2_ui.h"
#include "ui/aura/window.h"
#include "ui/base/ime/input_method_initializer.h"
#include "ui/native_theme/native_theme_aura.h"
#include "ui/views/linux_ui/linux_ui.h"
#endif

#include "content/nw/src/browser/shell_extension_system.h"
#include "content/nw/src/browser/shell_extension_system_factory.h"
#include "content/nw/src/browser/shell_extensions_browser_client.h"
#include "content/nw/src/common/shell_extensions_client.h"

#include "extensions/browser/browser_context_keyed_service_factories.h"
#include "extensions/browser/process_manager.h"

#include "chrome/browser/ui/app_modal/chrome_javascript_native_dialog_factory.h"

using base::MessageLoop;

namespace {

#if !defined(OS_WIN)
// Sets the file descriptor soft limit to |max_descriptors| or the OS hard
// limit, whichever is lower.
void SetFileDescriptorLimit(unsigned int max_descriptors) {
  struct rlimit limits;
  if (getrlimit(RLIMIT_NOFILE, &limits) == 0) {
    unsigned int new_limit = max_descriptors;
    if (limits.rlim_max > 0 && limits.rlim_max < max_descriptors) {
      new_limit = limits.rlim_max;
    }
    limits.rlim_cur = new_limit;
    if (setrlimit(RLIMIT_NOFILE, &limits) != 0) {
      PLOG(INFO) << "Failed to set file descriptor limit";
    }
  } else {
    PLOG(INFO) << "Failed to get file descriptor limit";
  }
}

#endif

base::StringPiece PlatformResourceProvider(int key) {
  if (key == IDR_DIR_HEADER_HTML) {
    base::StringPiece html_data =
        ui::ResourceBundle::GetSharedInstance().GetRawDataResource(
            IDR_DIR_HEADER_HTML);
    return html_data;
  }
  return base::StringPiece();
}

}  // namespace

namespace content {

ShellBrowserMainParts::ShellBrowserMainParts(
    const MainFunctionParams& parameters)
    : BrowserMainParts(),
      parameters_(parameters),
      run_message_loop_(true),
      notify_result_(ProcessSingleton::PROCESS_NONE)
{
#if defined(ENABLE_PRINTING)
  // Must be created after the NotificationService.
  print_job_manager_.reset(new printing::PrintJobManager);
#endif
}

ShellBrowserMainParts::~ShellBrowserMainParts() {
}

#if !defined(OS_MACOSX)
void ShellBrowserMainParts::PreMainMessageLoopStart() {
}
#endif

void ShellBrowserMainParts::PreMainMessageLoopRun() {

#if !defined(OS_MACOSX)
  Init();
#endif

  if (parameters_.ui_task) {
    parameters_.ui_task->Run();
    delete parameters_.ui_task;
    run_message_loop_ = false;
  }
}

bool ShellBrowserMainParts::MainMessageLoopRun(int* result_code)  {
  return !run_message_loop_;
}

void ShellBrowserMainParts::PostMainMessageLoopRun() {
  //  if (devtools_delegate_)
  // devtools_delegate_->Stop();
  // FIXME
  if (notify_result_ == ProcessSingleton::PROCESS_NONE)
    process_singleton_->Cleanup();

  devtools_http_handler_.reset();

  BrowserContextDependencyManager::GetInstance()->DestroyBrowserContextServices(
      browser_context_.get());
  extension_system_ = NULL;
  extensions::ExtensionsBrowserClient::Set(NULL);
  extensions_browser_client_.reset();

  browser_context_.reset();
  off_the_record_browser_context_.reset();
}

void ShellBrowserMainParts::PostMainMessageLoopStart() {
#if defined(TOOLKIT_GTK)
  printing::PrintingContextLinux::SetCreatePrintDialogFunction(
      &PrintDialogGtk::CreatePrintDialog);
#endif
}

int ShellBrowserMainParts::PreCreateThreads() {
  net::ProxyResolverV8::EnsureIsolateCreated();
#if defined(USE_AURA)
  gfx::Screen::SetScreenInstance(gfx::SCREEN_TYPE_NATIVE,
                                 views::CreateDesktopScreen());
#endif
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  package_.reset(new nw::Package());
  return 0;
}

void ShellBrowserMainParts::PostDestroyThreads() {
#if defined(USE_AURA)
  aura::Env::DeleteInstance();
  delete views::ViewsDelegate::views_delegate;
#endif
}

void ShellBrowserMainParts::ToolkitInitialized() {
#if defined(USE_AURA)
  aura::Env::CreateInstance(true);

  DCHECK(!views::ViewsDelegate::views_delegate);
  new views::DesktopTestViewsDelegate;
#endif

#if defined(OS_LINUX)
  views::LinuxUI::instance()->Initialize();
#endif
}

void ShellBrowserMainParts::Init() {
  //this will be reset to false before entering the message loop
  base::ThreadRestrictions::SetIOAllowed(true);

  browser_context_.reset(new ShellBrowserContext(false, package()));
  browser_context_->PreMainMessageLoopRun();

  off_the_record_browser_context_.reset(
      new ShellBrowserContext(true, package()));

  process_singleton_.reset(new ProcessSingleton(browser_context_->GetPath(),
                                                base::Bind(&ShellBrowserMainParts::ProcessSingletonNotificationCallback, base::Unretained(this))));
  notify_result_ = process_singleton_->NotifyOtherProcessOrCreate();


  // Quit if the other existing instance want to handle it.
  if (notify_result_ == ProcessSingleton::PROCESS_NOTIFIED) {
    MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
    return;
  }

  net::NetModule::SetResourceProvider(PlatformResourceProvider);

  extensions_client_.reset(CreateExtensionsClient());
  extensions::ExtensionsClient::Set(extensions_client_.get());

  extensions_browser_client_.reset(
      CreateExtensionsBrowserClient(browser_context_.get()));
  extensions::ExtensionsBrowserClient::Set(extensions_browser_client_.get());

  // Create our custom ExtensionSystem first because other
  // KeyedServices depend on it.
  // TODO(yoz): Move this after EnsureBrowserContextKeyedServiceFactoriesBuilt.
  CreateExtensionSystem();

  // Register additional KeyedService factories here. See
  // ChromeBrowserMainExtraPartsProfiles for details.
  extensions::EnsureBrowserContextKeyedServiceFactoriesBuilt();
  extensions::ShellExtensionSystemFactory::GetInstance();

  BrowserContextDependencyManager::GetInstance()->CreateBrowserContextServices(
      browser_context_.get());


  devtools_http_handler_.reset(
      ShellDevToolsManagerDelegate::CreateHttpHandler(browser_context_.get()));

  extensions::ShellExtensionSystem* extension_system = static_cast<extensions::ShellExtensionSystem*>(
                                extensions::ExtensionSystem::Get(browser_context_.get()));
  extension_system->Init();
  const extensions::Extension* extension = extension_system->LoadInternalApp();
  if (extension) {
    extension_system->LaunchApp(extension->id());
    extensions::ProcessManager::Get(browser_context_.get())->IncrementLazyKeepaliveCount(extension);
  }

  std::string dummy;
  if (!package()->root()->GetString("bg-script", &dummy) ||
      package()->root()->GetString(switches::kmMain, &dummy))
    Shell::Create(browser_context_.get(),
                package()->GetStartupURL(),
                NULL,
                MSG_ROUTING_NONE,
                NULL);

#if defined(OS_LINUX)
  InstallChromeJavaScriptNativeDialogFactory();
  nw::InstallConstrainedWindowViewsClient();
#endif
}

bool ShellBrowserMainParts::ProcessSingletonNotificationCallback(
    const CommandLine& command_line,
    const base::FilePath& current_directory) {

  // Don't reuse current instance if 'single-instance' is specified to false.
  bool single_instance;
  if (package_->root()->GetBoolean(switches::kmSingleInstance,
                                   &single_instance) &&
      !single_instance)
    return false;

#if defined(OS_WIN)
  std::string cmd = base::UTF16ToUTF8(command_line.GetCommandLineString());
#else
  std::string cmd = command_line.GetCommandLineString();
#endif
  static const char* const kSwitchNames[] = {
    switches::kNoSandbox,
    switches::kProcessPerTab,
    switches::kEnableExperimentalWebPlatformFeatures,
    //    switches::kEnableCssShaders,
    switches::kAllowFileAccessFromFiles,
  };
  for (size_t i = 0; i < arraysize(kSwitchNames); ++i) {
    ReplaceSubstringsAfterOffset(&cmd, 0, std::string(" --") + kSwitchNames[i], "");
  }

#if 0
  nwapi::App::EmitOpenEvent(UTF16ToUTF8(cmd));
#else
  nwapi::App::EmitOpenEvent(cmd);
#endif

  return true;
}

printing::PrintJobManager* ShellBrowserMainParts::print_job_manager() {
  // TODO(abarth): DCHECK(CalledOnValidThread());
  // http://code.google.com/p/chromium/issues/detail?id=6828
  // print_job_manager_ is initialized in the constructor and destroyed in the
  // destructor, so it should always be valid.
  DCHECK(print_job_manager_.get());
  return print_job_manager_.get();
}

void ShellBrowserMainParts::PreEarlyInitialization() {
#if !defined(OS_WIN)
  // see chrome_browser_main_posix.cc
  CommandLine& command_line = *CommandLine::ForCurrentProcess();
  const std::string fd_limit_string =
      command_line.GetSwitchValueASCII("file-descriptor-limit");
  int fd_limit = 0;
  if (!fd_limit_string.empty()) {
    base::StringToInt(fd_limit_string, &fd_limit);
  }
#if defined(OS_MACOSX)
  // We use quite a few file descriptors for our IPC, and the default limit on
  // the Mac is low (256), so bump it up if there is no explicit override.
  if (fd_limit == 0) {
    fd_limit = 1024;
  }
#endif  // OS_MACOSX
  if (fd_limit > 0)
    SetFileDescriptorLimit(fd_limit);
#endif // !OS_WIN

#if defined(OS_LINUX)
  views::LinuxUI* gtk2_ui = BuildGtk2UI();
  // gtk2_ui->SetNativeThemeOverride(base::Bind(&GetNativeThemeForWindow));
  views::LinuxUI::SetInstance(gtk2_ui);
  // ui::InitializeInputMethodForTesting();
#endif
}

extensions::ExtensionsClient* ShellBrowserMainParts::CreateExtensionsClient() {
  return new extensions::ShellExtensionsClient();
}

extensions::ExtensionsBrowserClient* ShellBrowserMainParts::CreateExtensionsBrowserClient(
    content::BrowserContext* context) {
  return new extensions::ShellExtensionsBrowserClient(context);
}

void ShellBrowserMainParts::CreateExtensionSystem() {
  DCHECK(browser_context_);
  extension_system_ = static_cast<extensions::ShellExtensionSystem*>(
            extensions::ExtensionSystem::Get(browser_context_.get()));
  extension_system_->InitForRegularProfile(true);
}

}  // namespace content
