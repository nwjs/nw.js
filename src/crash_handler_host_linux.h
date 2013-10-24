// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CRASH_HANDLER_HOST_LINUX_H_
#define CHROME_BROWSER_CRASH_HANDLER_HOST_LINUX_H_

#include <sys/types.h>

#include <string>

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_loop.h"

struct BreakpadInfo;

namespace base {
class Thread;
}

template <typename T> struct DefaultSingletonTraits;

// This is the base class for singleton objects which crash dump renderers and
// plugins on Linux or Android. We perform the crash dump from the browser
// because it allows us to be outside the sandbox.
//
// PluginCrashHandlerHostLinux and RendererCrashHandlerHostLinux are
// singletons that handle plugin and renderer crashes, respectively.
//
// Processes signal that they need to be dumped by sending a datagram over a
// UNIX domain socket. All processes of the same type share the client end of
// this socket which is installed in their descriptor table before exec.
class CrashHandlerHostLinux : public base::MessageLoopForIO::Watcher,
                              public base::MessageLoop::DestructionObserver {
 public:
  // Get the file descriptor which processes should be given in order to signal
  // crashes to the browser.
  int GetDeathSignalSocket() const {
    return process_socket_;
  }

  // MessagePumbLibevent::Watcher impl:
  virtual void OnFileCanWriteWithoutBlocking(int fd) OVERRIDE;
  virtual void OnFileCanReadWithoutBlocking(int fd) OVERRIDE;

  // MessageLoop::DestructionObserver impl:
  virtual void WillDestroyCurrentMessageLoop() OVERRIDE;

  // Whether we are shutting down or not.
  bool IsShuttingDown() const;

 protected:
  CrashHandlerHostLinux();
  virtual ~CrashHandlerHostLinux();

  // Only called in concrete subclasses.
  void InitCrashUploaderThread();

  std::string process_type_;

 private:
  void Init();

  // This is here on purpose to make CrashHandlerHostLinux abstract.
  virtual void SetProcessType() = 0;

  // Do work on the FILE thread for OnFileCanReadWithoutBlocking().
  void WriteDumpFile(BreakpadInfo* info,
                     pid_t crashing_pid,
                     char* crash_context,
                     int signal_fd);

  // Continue OnFileCanReadWithoutBlocking()'s work on the IO thread.
  void QueueCrashDumpTask(BreakpadInfo* info, int signal_fd);

  int process_socket_;
  int browser_socket_;

  base::MessageLoopForIO::FileDescriptorWatcher file_descriptor_watcher_;
  scoped_ptr<base::Thread> uploader_thread_;
  bool shutting_down_;

#if defined(ADDRESS_SANITIZER)
  char* asan_report_str_;
#endif

  DISALLOW_COPY_AND_ASSIGN(CrashHandlerHostLinux);
};

class ExtensionCrashHandlerHostLinux : public CrashHandlerHostLinux {
 public:
  // Returns the singleton instance.
  static ExtensionCrashHandlerHostLinux* GetInstance();

 private:
  friend struct DefaultSingletonTraits<ExtensionCrashHandlerHostLinux>;
  ExtensionCrashHandlerHostLinux();
  virtual ~ExtensionCrashHandlerHostLinux();

  virtual void SetProcessType() OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(ExtensionCrashHandlerHostLinux);
};

class GpuCrashHandlerHostLinux : public CrashHandlerHostLinux {
 public:
  // Returns the singleton instance.
  static GpuCrashHandlerHostLinux* GetInstance();

 private:
  friend struct DefaultSingletonTraits<GpuCrashHandlerHostLinux>;
  GpuCrashHandlerHostLinux();
  virtual ~GpuCrashHandlerHostLinux();

  virtual void SetProcessType() OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(GpuCrashHandlerHostLinux);
};

class PluginCrashHandlerHostLinux : public CrashHandlerHostLinux {
 public:
  // Returns the singleton instance.
  static PluginCrashHandlerHostLinux* GetInstance();

 private:
  friend struct DefaultSingletonTraits<PluginCrashHandlerHostLinux>;
  PluginCrashHandlerHostLinux();
  virtual ~PluginCrashHandlerHostLinux();

  virtual void SetProcessType() OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(PluginCrashHandlerHostLinux);
};

class PpapiCrashHandlerHostLinux : public CrashHandlerHostLinux {
 public:
  // Returns the singleton instance.
  static PpapiCrashHandlerHostLinux* GetInstance();

 private:
  friend struct DefaultSingletonTraits<PpapiCrashHandlerHostLinux>;
  PpapiCrashHandlerHostLinux();
  virtual ~PpapiCrashHandlerHostLinux();

  virtual void SetProcessType() OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(PpapiCrashHandlerHostLinux);
};

class RendererCrashHandlerHostLinux : public CrashHandlerHostLinux {
 public:
  // Returns the singleton instance.
  static RendererCrashHandlerHostLinux* GetInstance();

 private:
  friend struct DefaultSingletonTraits<RendererCrashHandlerHostLinux>;
  RendererCrashHandlerHostLinux();
  virtual ~RendererCrashHandlerHostLinux();

  virtual void SetProcessType() OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(RendererCrashHandlerHostLinux);
};

#endif  // CHROME_BROWSER_CRASH_HANDLER_HOST_LINUX_H_
