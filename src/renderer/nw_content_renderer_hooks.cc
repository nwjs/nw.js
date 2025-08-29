#include "nw_content_renderer_hooks.h"

// base
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/native_library.h"
#include "base/strings/utf_string_conversions.h"

#if defined(OS_MAC)
#include "base/apple/bundle_locations.h"
#endif

#define CONTENT_IMPLEMENTATION 1
#include "content/common/content_export.h"
#include "content/public/common/content_switches.h"

// v8
#include "v8/include/v8.h"

// third_party/node
// FIXME: node_webkit.h should include v8 headers
#include "third_party/node-nw/src/node_webkit.h"

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#if defined(COMPONENT_BUILD) && defined(WIN32)
#define NW_HOOK_MAP(type, sym, fn) BASE_EXPORT type fn;
#else
#define NW_HOOK_MAP(type, sym, fn) extern type fn;
#endif
#include "content/nw/src/common/node_hooks.h"
#undef NW_HOOK_MAP

namespace nw {

// renderer
void LoadNodeSymbols() {
  base::NativeLibraryLoadError error;
  std::string node_dllname("node");
  const base::CommandLine* cmdline = base::CommandLine::ForCurrentProcess();
  if (cmdline->HasSwitch(switches::kNodeDllName)) {
    node_dllname = cmdline->GetSwitchValueASCII(switches::kNodeDllName);
  }
#if defined(OS_MAC)
  base::FilePath node_dll_path = base::apple::FrameworkBundlePath().Append(base::FilePath::FromUTF8Unsafe(base::GetNativeLibraryName(node_dllname)));
#else
  base::FilePath node_dll_path = base::FilePath::FromUTF8Unsafe(base::GetNativeLibraryName(node_dllname));
#endif
  base::NativeLibrary node_dll = base::LoadNativeLibrary(node_dll_path, &error);
  if(!node_dll)
    LOG_IF(FATAL, true) << "Failed to load node library (error: " << error.ToString() << ")";
  else {
    #define NW_HOOK_MAP(type, sym, fn) fn = (type) base::GetFunctionPointerFromNativeLibrary(node_dll, sym); \
                                       DCHECK(fn) << "Unable to find symbol for " << sym;
    #include "content/nw/src/common/node_hooks.h"
    #undef NW_HOOK_MAP
  }
}

void KickNextTick() {
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent()->GetCurrentContext(),
				 v8::MicrotasksScope::kDoNotRunMicrotasks);
  void* env = g_get_node_env_fn();
  if (env)
    g_call_tick_callback_fn(env);
}

} // nw
