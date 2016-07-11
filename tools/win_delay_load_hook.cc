/*
 * When this file is linked to a DLL, it sets up a delay-load hook that
 * intervenes when the DLL is trying to load 'node.exe' or 'iojs.exe'
 * dynamically. Instead of trying to locate the .exe file it'll just return
 * a handle to the process image.
 *
 * This allows compiled addons to work when node.exe or iojs.exe is renamed.
 */

#ifdef _MSC_VER

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <delayimp.h>
#include <string.h>

static HMODULE node_dll = NULL;
static HMODULE nw_dll = NULL;

static FARPROC WINAPI load_exe_hook(unsigned int event, DelayLoadInfo* info) {
  if (event == dliNotePreGetProcAddress) {
    FARPROC ret = NULL;
    ret = GetProcAddress(node_dll, info->dlp.szProcName);
    if (ret)
      return ret;
    ret = GetProcAddress(nw_dll, info->dlp.szProcName);
    return ret;
  }
  if (event == dliStartProcessing) {
    node_dll = GetModuleHandle("node.dll");
    nw_dll = GetModuleHandle("nw.dll");
    return NULL;
  }
  if (event != dliNotePreLoadLibrary)
    return NULL;

  if (_stricmp(info->szDll, "iojs.exe") != 0 &&
      _stricmp(info->szDll, "node.exe") != 0)
    return NULL;

  return (FARPROC) node_dll;
}

decltype(__pfnDliNotifyHook2) __pfnDliNotifyHook2 = load_exe_hook;

#endif
