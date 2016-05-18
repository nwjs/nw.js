#include "nw_content_renderer_hooks.h"

// base
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/native_library.h"
#include "base/strings/utf_string_conversions.h"

#if defined(OS_MACOSX)
#include "base/mac/bundle_locations.h"
#endif

// v8
#include "v8/include/v8.h"

// third_party/node
// FIXME: node_webkit.h should include v8 headers
#include "third_party/node/src/node_webkit.h"

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#define NW_HOOK_MAP(type, sym, fn) extern type fn;
#define NW_HOOK_INIT(type, sym, fn) type fn = nullptr;
#include "content/nw/src/common/node_hooks.h"
#undef NW_HOOK_INIT
#undef NW_HOOK_MAP

namespace nw {

// renderer
void LoadNodeSymbols() {
  base::NativeLibraryLoadError error;
#if defined(OS_MACOSX)
  base::FilePath node_dll_path = base::mac::FrameworkBundlePath().Append(base::FilePath::FromUTF16Unsafe(base::GetNativeLibraryName(base::UTF8ToUTF16("libnode"))));
#else
  base::FilePath node_dll_path = base::FilePath::FromUTF16Unsafe(base::GetNativeLibraryName(base::UTF8ToUTF16("node")));
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
  v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
  void* env = g_get_node_env_fn();
  if (env)
    g_call_tick_callback_fn(env);
}

} // nw