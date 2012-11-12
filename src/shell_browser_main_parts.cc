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
#include "base/message_loop.h"
#include "base/string_number_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "content/nw/src/api/app/app.h"
#include "content/nw/src/browser/shell_devtools_delegate.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/main_function_params.h"
#include "grit/net_resources.h"
#include "net/base/net_module.h"
#include "ui/base/resource/resource_bundle.h"

namespace {

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
      devtools_delegate_(NULL) {
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
  if (devtools_delegate_)
    devtools_delegate_->Stop();
  browser_context_.reset();
  off_the_record_browser_context_.reset();
}

void ShellBrowserMainParts::Init() {
  package_.reset(new nw::Package());

  browser_context_.reset(new ShellBrowserContext(false, package()));
  off_the_record_browser_context_.reset(
      new ShellBrowserContext(true, package()));

  // OS X automatically handles multi process issue for us, so we don't need
  // to check for other instances of node-webkit on Mac.
#if !defined(MAC_OSX)
  process_singleton_.reset(new ProcessSingleton(browser_context_->GetPath()));
  ProcessSingleton::NotifyResult result =
      process_singleton_->NotifyOtherProcessOrCreate(
        base::Bind(&ShellBrowserMainParts::ProcessSingletonNotificationCallback,
                   base::Unretained(this)));

  // Quit if the other existing instance want to handle it.
  if (result == ProcessSingleton::PROCESS_NOTIFIED) {
    MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
    return;
  }
#endif

  net::NetModule::SetResourceProvider(PlatformResourceProvider);

  int port = 0;
  // See if the user specified a port on the command line (useful for
  // automation). If not, use an ephemeral port by specifying 0.
  CommandLine& command_line = *CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kRemoteDebuggingPort)) {
    int temp_port;
    std::string port_str =
        command_line.GetSwitchValueASCII(switches::kRemoteDebuggingPort);
    if (base::StringToInt(port_str, &temp_port) &&
        temp_port > 0 && temp_port < 65535) {
      port = temp_port;
    } else {
      DLOG(WARNING) << "Invalid http debugger port number " << temp_port;
    }
  }
  devtools_delegate_ = new ShellDevToolsDelegate(browser_context_.get(), port);

  Shell::Create(browser_context_.get(),
                package()->GetStartupURL(),
                NULL,
                MSG_ROUTING_NONE,
                NULL);
}

bool ShellBrowserMainParts::ProcessSingletonNotificationCallback(
    const CommandLine& command_line,
    const FilePath& current_directory) {
  if (!package_->self_extract()) {
    // We're in runtime mode, create the new app.
    return false;
  }

  // Don't reuse current instance if 'single-instance' is specified to false.
  bool single_instance;
  if (package_->root()->GetBoolean(switches::kmSingleInstance,
                                   &single_instance) &&
      !single_instance)
    return false;

  CommandLine::StringVector args = command_line.GetArgs();

  // Send open event one by one.
  for (size_t i = 0; i < args.size(); ++i) { 
#if defined(OS_WIN)
    api::App::EmitOpenEvent(UTF16ToUTF8(args[i]));
#else
    api::App::EmitOpenEvent(args[i]);
#endif
  }

  return true;
}

}  // namespace content
