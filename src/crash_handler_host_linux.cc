// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/crash_handler_host_linux.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/files/file_path.h"
#include "base/format_macros.h"
#include "base/linux_util.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/message_loop/message_loop.h"
#include "base/path_service.h"
#include "base/posix/eintr_wrapper.h"
#include "base/rand_util.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread.h"
#include "breakpad/src/client/linux/handler/exception_handler.h"
#include "breakpad/src/client/linux/minidump_writer/linux_dumper.h"
#include "breakpad/src/client/linux/minidump_writer/minidump_writer.h"
#include "chrome/app/breakpad_linux_impl.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/env_vars.h"
#include "content/public/browser/browser_thread.h"

#if defined(OS_ANDROID)
#include <sys/linux-syscalls.h>

#define SYS_read __NR_read
#endif

using content::BrowserThread;
using google_breakpad::ExceptionHandler;

namespace {

// The length of the control message:
const unsigned kControlMsgSize =
    CMSG_SPACE(2*sizeof(int)) + CMSG_SPACE(sizeof(struct ucred));
// The length of the regular payload:
const unsigned kCrashContextSize = sizeof(ExceptionHandler::CrashContext);

// Handles the crash dump and frees the allocated BreakpadInfo struct.
void CrashDumpTask(CrashHandlerHostLinux* handler, BreakpadInfo* info) {
  if (handler->IsShuttingDown())
    return;

  HandleCrashDump(*info);
  delete[] info->filename;
  delete[] info->process_type;
  delete[] info->crash_url;
  delete[] info->guid;
  delete[] info->distro;
  delete info->crash_keys;
  delete info;
}

}  // namespace

// Since classes derived from CrashHandlerHostLinux are singletons, it's only
// destroyed at the end of the processes lifetime, which is greater in span than
// the lifetime of the IO message loop. Thus, all calls to base::Bind() use
// non-refcounted pointers.

CrashHandlerHostLinux::CrashHandlerHostLinux()
    : shutting_down_(false) {
  int fds[2];
  // We use SOCK_SEQPACKET rather than SOCK_DGRAM to prevent the process from
  // sending datagrams to other sockets on the system. The sandbox may prevent
  // the process from calling socket() to create new sockets, but it'll still
  // inherit some sockets. With PF_UNIX+SOCK_DGRAM, it can call sendmsg to send
  // a datagram to any (abstract) socket on the same system. With
  // SOCK_SEQPACKET, this is prevented.
  CHECK_EQ(socketpair(AF_UNIX, SOCK_SEQPACKET, 0, fds), 0);
  static const int on = 1;

  // Enable passcred on the server end of the socket
  CHECK_EQ(setsockopt(fds[1], SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)), 0);

  process_socket_ = fds[0];
  browser_socket_ = fds[1];

  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&CrashHandlerHostLinux::Init, base::Unretained(this)));
}

CrashHandlerHostLinux::~CrashHandlerHostLinux() {
  (void) HANDLE_EINTR(close(process_socket_));
  (void) HANDLE_EINTR(close(browser_socket_));
}

void CrashHandlerHostLinux::Init() {
  base::MessageLoopForIO* ml = base::MessageLoopForIO::current();
  CHECK(ml->WatchFileDescriptor(
      browser_socket_, true /* persistent */,
      base::MessageLoopForIO::WATCH_READ,
      &file_descriptor_watcher_, this));
  ml->AddDestructionObserver(this);
}

void CrashHandlerHostLinux::InitCrashUploaderThread() {
  SetProcessType();
  uploader_thread_.reset(
      new base::Thread(std::string(process_type_ + "_crash_uploader").c_str()));
  uploader_thread_->Start();
}

void CrashHandlerHostLinux::OnFileCanWriteWithoutBlocking(int fd) {
  NOTREACHED();
}

void CrashHandlerHostLinux::OnFileCanReadWithoutBlocking(int fd) {
  DCHECK_EQ(fd, browser_socket_);

  // A process has crashed and has signaled us by writing a datagram
  // to the death signal socket. The datagram contains the crash context needed
  // for writing the minidump as well as a file descriptor and a credentials
  // block so that they can't lie about their pid.
  //
  // The message sender is in chrome/app/breakpad_linux.cc.

  struct msghdr msg = {0};
  struct iovec iov[kCrashIovSize];

  // Freed in WriteDumpFile();
  char* crash_context = new char[kCrashContextSize];
  // Freed in CrashDumpTask();
  char* guid = new char[kGuidSize + 1];
  char* crash_url = new char[kMaxActiveURLSize + 1];
  char* distro = new char[kDistroSize + 1];
#if defined(ADDRESS_SANITIZER)
  asan_report_str_ = new char[kMaxAsanReportSize + 1];
#endif

  // Freed in CrashDumpTask().
  CrashKeyStorage* crash_keys = new CrashKeyStorage;
  google_breakpad::SerializedNonAllocatingMap* serialized_crash_keys;
  size_t crash_keys_size = crash_keys->Serialize(
      const_cast<const google_breakpad::SerializedNonAllocatingMap**>(
          &serialized_crash_keys));

  char* tid_buf_addr = NULL;
  int tid_fd = -1;
  uint64_t uptime;
  size_t oom_size;
  char control[kControlMsgSize];
  const ssize_t expected_msg_size =
      kCrashContextSize +
      kGuidSize + 1 +
      kMaxActiveURLSize + 1 +
      kDistroSize + 1 +
      sizeof(tid_buf_addr) + sizeof(tid_fd) +
      sizeof(uptime) +
#if defined(ADDRESS_SANITIZER)
      kMaxAsanReportSize + 1 +
#endif
      sizeof(oom_size) +
      crash_keys_size;
  iov[0].iov_base = crash_context;
  iov[0].iov_len = kCrashContextSize;
  iov[1].iov_base = guid;
  iov[1].iov_len = kGuidSize + 1;
  iov[2].iov_base = crash_url;
  iov[2].iov_len = kMaxActiveURLSize + 1;
  iov[3].iov_base = distro;
  iov[3].iov_len = kDistroSize + 1;
  iov[4].iov_base = &tid_buf_addr;
  iov[4].iov_len = sizeof(tid_buf_addr);
  iov[5].iov_base = &tid_fd;
  iov[5].iov_len = sizeof(tid_fd);
  iov[6].iov_base = &uptime;
  iov[6].iov_len = sizeof(uptime);
  iov[7].iov_base = &oom_size;
  iov[7].iov_len = sizeof(oom_size);
  iov[8].iov_base = serialized_crash_keys;
  iov[8].iov_len = crash_keys_size;
#if defined(ADDRESS_SANITIZER)
  iov[9].iov_base = asan_report_str_;
  iov[9].iov_len = kMaxAsanReportSize + 1;
#endif
  msg.msg_iov = iov;
  msg.msg_iovlen = kCrashIovSize;
  msg.msg_control = control;
  msg.msg_controllen = kControlMsgSize;

  const ssize_t msg_size = HANDLE_EINTR(recvmsg(browser_socket_, &msg, 0));
  if (msg_size != expected_msg_size) {
    LOG(ERROR) << "Error reading from death signal socket. Crash dumping"
               << " is disabled."
               << " msg_size:" << msg_size
               << " errno:" << errno;
    file_descriptor_watcher_.StopWatchingFileDescriptor();
    return;
  }

  if (msg.msg_controllen != kControlMsgSize ||
      msg.msg_flags & ~MSG_TRUNC) {
    LOG(ERROR) << "Received death signal message with the wrong size;"
               << " msg.msg_controllen:" << msg.msg_controllen
               << " msg.msg_flags:" << msg.msg_flags
               << " kCrashContextSize:" << kCrashContextSize
               << " kControlMsgSize:" << kControlMsgSize;
    return;
  }

  // Walk the control payload an extract the file descriptor and validated pid.
  pid_t crashing_pid = -1;
  int partner_fd = -1;
  int signal_fd = -1;
  for (struct cmsghdr *hdr = CMSG_FIRSTHDR(&msg); hdr;
       hdr = CMSG_NXTHDR(&msg, hdr)) {
    if (hdr->cmsg_level != SOL_SOCKET)
      continue;
    if (hdr->cmsg_type == SCM_RIGHTS) {
      const unsigned len = hdr->cmsg_len -
          (((uint8_t*)CMSG_DATA(hdr)) - (uint8_t*)hdr);
      DCHECK_EQ(len % sizeof(int), 0u);
      const unsigned num_fds = len / sizeof(int);
      if (num_fds != 2) {
        // A nasty process could try and send us too many descriptors and
        // force a leak.
        LOG(ERROR) << "Death signal contained wrong number of descriptors;"
                   << " num_fds:" << num_fds;
        for (unsigned i = 0; i < num_fds; ++i)
          (void) HANDLE_EINTR(close(reinterpret_cast<int*>(CMSG_DATA(hdr))[i]));
        return;
      } else {
        partner_fd = reinterpret_cast<int*>(CMSG_DATA(hdr))[0];
        signal_fd = reinterpret_cast<int*>(CMSG_DATA(hdr))[1];
      }
    } else if (hdr->cmsg_type == SCM_CREDENTIALS) {
      const struct ucred *cred =
          reinterpret_cast<struct ucred*>(CMSG_DATA(hdr));
      crashing_pid = cred->pid;
    }
  }

  if (crashing_pid == -1 || partner_fd == -1 || signal_fd == -1) {
    LOG(ERROR) << "Death signal message didn't contain all expected control"
               << " messages";
    if (partner_fd >= 0)
      (void) HANDLE_EINTR(close(partner_fd));
    if (signal_fd >= 0)
      (void) HANDLE_EINTR(close(signal_fd));
    return;
  }

  // Kernel bug workaround (broken in 2.6.30 and 2.6.32, working in 2.6.38).
  // The kernel doesn't translate PIDs in SCM_CREDENTIALS across PID
  // namespaces. Thus |crashing_pid| might be garbage from our point of view.
  // In the future we can remove this workaround, but we have to wait a couple
  // of years to be sure that it's worked its way out into the world.
  // TODO(thestig) Remove the workaround when Ubuntu Lucid is deprecated.

  // The crashing process closes its copy of the signal_fd immediately after
  // calling sendmsg(). We can thus not reliably look for with with
  // FindProcessHoldingSocket(). But by necessity, it has to keep the
  // partner_fd open until the crashdump is complete.
  ino_t inode_number;
  if (!base::FileDescriptorGetInode(&inode_number, partner_fd)) {
    LOG(WARNING) << "Failed to get inode number for passed socket";
    (void) HANDLE_EINTR(close(partner_fd));
    (void) HANDLE_EINTR(close(signal_fd));
    return;
  }
  (void) HANDLE_EINTR(close(partner_fd));

  pid_t actual_crashing_pid = -1;
  if (!base::FindProcessHoldingSocket(&actual_crashing_pid, inode_number)) {
    LOG(WARNING) << "Failed to find process holding other end of crash reply "
                    "socket";
    (void) HANDLE_EINTR(close(signal_fd));
    return;
  }

  crashing_pid = actual_crashing_pid;

  // The crashing TID set inside the compromised context via
  // sys_gettid() in ExceptionHandler::HandleSignal might be wrong (if
  // the kernel supports PID namespacing) and may need to be
  // translated.
  //
  // We expect the crashing thread to be in sys_read(), waiting for us to
  // write to |signal_fd|. Most newer kernels where we have the different pid
  // namespaces also have /proc/[pid]/syscall, so we can look through
  // |actual_crashing_pid|'s thread group and find the thread that's in the
  // read syscall with the right arguments.

  std::string expected_syscall_data;
  // /proc/[pid]/syscall is formatted as follows:
  // syscall_number arg1 ... arg6 sp pc
  // but we just check syscall_number through arg3.
  base::StringAppendF(&expected_syscall_data, "%d 0x%x %p 0x1 ",
                      SYS_read, tid_fd, tid_buf_addr);
  bool syscall_supported = false;
  pid_t crashing_tid =
      base::FindThreadIDWithSyscall(crashing_pid,
                                    expected_syscall_data,
                                    &syscall_supported);
  if (crashing_tid == -1) {
    // We didn't find the thread we want. Maybe it didn't reach
    // sys_read() yet or the thread went away.  We'll just take a
    // guess here and assume the crashing thread is the thread group
    // leader.  If procfs syscall is not supported by the kernel, then
    // we assume the kernel also does not support TID namespacing and
    // trust the TID passed by the crashing process.
    LOG(WARNING) << "Could not translate tid - assuming crashing thread is "
        "thread group leader; syscall_supported=" << syscall_supported;
    crashing_tid = crashing_pid;
  }

  ExceptionHandler::CrashContext* bad_context =
      reinterpret_cast<ExceptionHandler::CrashContext*>(crash_context);
  bad_context->tid = crashing_tid;

  // Sanitize the string data a bit more
  guid[kGuidSize] = crash_url[kMaxActiveURLSize] = distro[kDistroSize] = 0;

  // Freed in CrashDumpTask();
  BreakpadInfo* info = new BreakpadInfo;

  info->fd = -1;
  info->process_type_length = process_type_.length();
  char* process_type_str = new char[info->process_type_length + 1];
  process_type_.copy(process_type_str, info->process_type_length);
  process_type_str[info->process_type_length] = '\0';
  info->process_type = process_type_str;

  info->crash_url_length = strlen(crash_url);
  info->crash_url = crash_url;

  info->guid_length = strlen(guid);
  info->guid = guid;

  info->distro_length = strlen(distro);
  info->distro = distro;
#if defined(OS_ANDROID)
  // Nothing gets uploaded in android.
  info->upload = false;
#else
  info->upload = (getenv(env_vars::kHeadless) == NULL);
#endif

  info->crash_keys = crash_keys;

#if defined(ADDRESS_SANITIZER)
  info->asan_report_str = asan_report_str_;
  info->asan_report_length = strlen(asan_report_str_);
#endif
  info->process_start_time = uptime;
  info->oom_size = oom_size;

  BrowserThread::PostTask(
      BrowserThread::FILE, FROM_HERE,
      base::Bind(&CrashHandlerHostLinux::WriteDumpFile,
                 base::Unretained(this),
                 info,
                 crashing_pid,
                 crash_context,
                 signal_fd));
}

void CrashHandlerHostLinux::WriteDumpFile(BreakpadInfo* info,
                                          pid_t crashing_pid,
                                          char* crash_context,
                                          int signal_fd) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));

  base::FilePath dumps_path("/tmp");
  PathService::Get(base::DIR_TEMP, &dumps_path);
  if (!info->upload)
    PathService::Get(chrome::DIR_CRASH_DUMPS, &dumps_path);
  const uint64 rand = base::RandUint64();
  const std::string minidump_filename =
      base::StringPrintf("%s/chromium-%s-minidump-%016" PRIx64 ".dmp",
                         dumps_path.value().c_str(),
                         process_type_.c_str(),
                         rand);

  if (!google_breakpad::WriteMinidump(minidump_filename.c_str(),
                                      kMaxMinidumpFileSize,
                                      crashing_pid, crash_context,
                                      kCrashContextSize,
                                      google_breakpad::MappingList(),
                                      google_breakpad::AppMemoryList())) {
    LOG(ERROR) << "Failed to write crash dump for pid " << crashing_pid;
  }
#if defined(ADDRESS_SANITIZER)
  // Create a temporary file holding the AddressSanitizer report.
  const std::string log_filename =
      base::StringPrintf("%s/chromium-%s-minidump-%016" PRIx64 ".log",
                         dumps_path.value().c_str(),
                         process_type_.c_str(),
                         rand);
  FILE* logfile = fopen(log_filename.c_str(), "w");
  CHECK(logfile);
  fprintf(logfile, "%s", asan_report_str_);
  fclose(logfile);
#endif

  delete[] crash_context;

  // Freed in CrashDumpTask();
  char* minidump_filename_str = new char[minidump_filename.length() + 1];
  minidump_filename.copy(minidump_filename_str, minidump_filename.length());
  minidump_filename_str[minidump_filename.length()] = '\0';
  info->filename = minidump_filename_str;
#if defined(ADDRESS_SANITIZER)
  char* minidump_log_filename_str = new char[minidump_filename.length() + 1];
  minidump_filename.copy(minidump_log_filename_str, minidump_filename.length());
  memcpy(minidump_log_filename_str + minidump_filename.length() - 3, "log", 3);
  minidump_log_filename_str[minidump_filename.length()] = '\0';
  info->log_filename = minidump_log_filename_str;
#endif
  info->pid = crashing_pid;

  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&CrashHandlerHostLinux::QueueCrashDumpTask,
                 base::Unretained(this),
                 info,
                 signal_fd));
}

void CrashHandlerHostLinux::QueueCrashDumpTask(BreakpadInfo* info,
                                               int signal_fd) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  // Send the done signal to the process: it can exit now.
  struct msghdr msg = {0};
  struct iovec done_iov;
  done_iov.iov_base = const_cast<char*>("\x42");
  done_iov.iov_len = 1;
  msg.msg_iov = &done_iov;
  msg.msg_iovlen = 1;

  (void) HANDLE_EINTR(sendmsg(signal_fd, &msg, MSG_DONTWAIT | MSG_NOSIGNAL));
  (void) HANDLE_EINTR(close(signal_fd));

  uploader_thread_->message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&CrashDumpTask, base::Unretained(this), info));
}

void CrashHandlerHostLinux::WillDestroyCurrentMessageLoop() {
  file_descriptor_watcher_.StopWatchingFileDescriptor();

  // If we are quitting and there are crash dumps in the queue, turn them into
  // no-ops.
  shutting_down_ = true;
  uploader_thread_->Stop();
}

bool CrashHandlerHostLinux::IsShuttingDown() const {
  return shutting_down_;
}

ExtensionCrashHandlerHostLinux::ExtensionCrashHandlerHostLinux() {
  InitCrashUploaderThread();
}

ExtensionCrashHandlerHostLinux::~ExtensionCrashHandlerHostLinux() {
}

void ExtensionCrashHandlerHostLinux::SetProcessType() {
  process_type_ = "extension";
}

// static
ExtensionCrashHandlerHostLinux* ExtensionCrashHandlerHostLinux::GetInstance() {
  return Singleton<ExtensionCrashHandlerHostLinux>::get();
}

GpuCrashHandlerHostLinux::GpuCrashHandlerHostLinux() {
  InitCrashUploaderThread();
}

GpuCrashHandlerHostLinux::~GpuCrashHandlerHostLinux() {
}

void GpuCrashHandlerHostLinux::SetProcessType() {
  process_type_ = "gpu-process";
}

// static
GpuCrashHandlerHostLinux* GpuCrashHandlerHostLinux::GetInstance() {
  return Singleton<GpuCrashHandlerHostLinux>::get();
}

PluginCrashHandlerHostLinux::PluginCrashHandlerHostLinux() {
  InitCrashUploaderThread();
}

PluginCrashHandlerHostLinux::~PluginCrashHandlerHostLinux() {
}

void PluginCrashHandlerHostLinux::SetProcessType() {
  process_type_ = "plugin";
}

// static
PluginCrashHandlerHostLinux* PluginCrashHandlerHostLinux::GetInstance() {
  return Singleton<PluginCrashHandlerHostLinux>::get();
}

PpapiCrashHandlerHostLinux::PpapiCrashHandlerHostLinux() {
  InitCrashUploaderThread();
}

PpapiCrashHandlerHostLinux::~PpapiCrashHandlerHostLinux() {
}

void PpapiCrashHandlerHostLinux::SetProcessType() {
  process_type_ = "ppapi";
}

// static
PpapiCrashHandlerHostLinux* PpapiCrashHandlerHostLinux::GetInstance() {
  return Singleton<PpapiCrashHandlerHostLinux>::get();
}

RendererCrashHandlerHostLinux::RendererCrashHandlerHostLinux() {
  InitCrashUploaderThread();
}

RendererCrashHandlerHostLinux::~RendererCrashHandlerHostLinux() {
}

void RendererCrashHandlerHostLinux::SetProcessType() {
  process_type_ = "renderer";
}

// static
RendererCrashHandlerHostLinux* RendererCrashHandlerHostLinux::GetInstance() {
  return Singleton<RendererCrashHandlerHostLinux>::get();
}
