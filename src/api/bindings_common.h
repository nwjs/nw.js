// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the \"Software\"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef CONTENT_NW_SRC_API_BINDINGS_COMMON_H_
#define CONTENT_NW_SRC_API_BINDINGS_COMMON_H_

#include <string>

#include "base/strings/string_piece.h"
#include "v8/include/v8.h"

namespace content {
class RenderView;
}

// ---------------------------------------------------------------------------
// ARM prebuilt binary support (nw.js issue #1151)
//
// nw.js historically shipped prebuilt binaries for x86/x64 only. Issue #1151
// requests prebuilt binaries for ARM devices, distinguishing the two common
// ARM ABIs:
//   * armel  - ARM EABI (soft-float), legacy 32-bit ARM devices.
//   * armhf  - ARM hard-float EABI, most 32-bit ARMv7 devices (Raspberry Pi,
//             BeagleBone, etc.).
//   * arm64  - ARMv8 64-bit (AArch64).
//
// This header exposes compile-time macros and run-time helpers so JS bindings
// and embedders can detect the running architecture and select the correct
// prebuilt native module / binary variant at load time.
//
// Compile-time macros (defined by the toolchain; mutually exclusive):
//   BINDINGS_ARCH_ARMEL   - 32-bit ARM soft-float EABI
//   BINDINGS_ARCH_ARMHF   - 32-bit ARM hard-float EABI
//   BINDINGS_ARCH_ARM64   - 64-bit ARM (AArch64)
//   BINDINGS_ARCH_X86     - 32-bit Intel/AMD
//   BINDINGS_ARCH_X64     - 64-bit Intel/AMD
//   BINDINGS_ARCH_UNKNOWN - anything else
// ---------------------------------------------------------------------------

#if defined(__aarch64__) || defined(_M_ARM64)
  #define BINDINGS_ARCH_ARM64 1
#elif defined(__arm__) || defined(__thumb__) || defined(_M_ARM)
  // Use __ARM_PCS_VFP to distinguish hard-float (armhf) from soft-float (armel).
  // Note: __ARM_EABI__ is defined for both ABIs and must not be used here.
  #if defined(__ARM_PCS_VFP)
    #define BINDINGS_ARCH_ARMHF 1
  #else
    #define BINDINGS_ARCH_ARMEL 1
  #endif
#elif defined(__i386__) || defined(_M_IX86)
  #define BINDINGS_ARCH_X86 1
#elif defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
  #define BINDINGS_ARCH_X64 1
#else
  #define BINDINGS_ARCH_UNKNOWN 1
#endif

// Compile-time verification: exactly one architecture macro must be defined.
#if defined(BINDINGS_ARCH_ARMEL) + defined(BINDINGS_ARCH_ARMHF) + defined(BINDINGS_ARCH_ARM64) + defined(BINDINGS_ARCH_X86) + defined(BINDINGS_ARCH_X64) + defined(BINDINGS_ARCH_UNKNOWN) != 1
  #error "Exactly one BINDINGS_ARCH_* macro must be defined."
#endif

// Returns a human-readable architecture identifier: "armel", "armhf", "arm64",
// "x86", "x64" or "unknown". Mirrors the prebuilt artifact naming convention
// used by nw.js release builds.
inline std::string GetCurrentCPUArchitecture() {
#if defined(BINDINGS_ARCH_ARMEL)
  return "armel";
#elif defined(BINDINGS_ARCH_ARMHF)
  return "armhf";
#elif defined(BINDINGS_ARCH_ARM64)
  return "arm64";
#elif defined(BINDINGS_ARCH_X86)
  return "x86";
#elif defined(BINDINGS_ARCH_X64)
  return "x64";
#else
  return "unknown";
#endif
}

// Returns true when running on any ARM variant (32-bit or 64-bit).
inline bool IsARM() {
#if defined(BINDINGS_ARCH_ARMEL) || defined(BINDINGS_ARCH_ARMHF) || defined(BINDINGS_ARCH_ARM64)
  return true;
#else
  return false;
#endif
}

// Returns true when running on a 32-bit ARM hard-float device (the most common
// ARM prebuilt target, e.g. Raspberry Pi).
inline bool IsARMHF() {
#if defined(BINDINGS_ARCH_ARMHF)
  return true;
#else
  return false;
#endif
}

// Returns true when running on a 32-bit ARM soft-float device (legacy armel).
inline bool IsARMEL() {
#if defined(BINDINGS_ARCH_ARMEL)
  return true;
#else
  return false;
#endif
}

// Get RenderView from current js context (only works under window context).
content::RenderView* GetCurrentRenderView();
content::RenderView* GetEnteredRenderView();

// Get string from resource_id.
base::StringPiece GetStringResource(int resource_id);

namespace remote {

v8::Handle<v8::Value> AllocateId(int routing_id);

// Tell browser to allocate a new object.
// function AllocateObject(id, name, options);
v8::Handle<v8::Value> AllocateObject(int routing_id,
                                     int object_id,
                                     const std::string& type,
                                     v8::Handle<v8::Value> options);

// Tell browser to delete a object.
// function DeallocateObject(id);
v8::Handle<v8::Value> DeallocateObject(int routing_id,
                                       int object_id);

// Call method of an object in browser.
// function CallObjectMethod(id, type, method, args);
v8::Handle<v8::Value> CallObjectMethod(int routing_id,
                                       int object_id,
                                       const std::string& type,
                                       const std::string& method,
                                       v8::Handle<v8::Value> args);

// Call method of an object in browser and return the result.
// function CallObjectMethod(id, type, method, args);
v8::Handle<v8::Value> CallObjectMethodSync(int routing_id,
                                           int object_id,
                                           const std::string& type,
                                           const std::string& method,
                                           v8::Handle<v8::Value> args);

}  // namespace remote

// ---------------------------------------------------------------------------
// Unit tests / verification (bindings_common_unittest)
// ---------------------------------------------------------------------------
//
// Compiled only when CONTENT_NW_SRC_API_BINDINGS_COMMON_UNITTEST is defined.
//
// Example standalone driver (bindings_common_unittest.cc):
//   #define CONTENT_NW_SRC_API_BINDINGS_COMMON_UNITTEST
//   #include "src/api/bindings_common.h"
//   #include <gtest/gtest.h>
//   int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
//   }
//
// Expected outcome: exactly one architecture macro is defined, the string
// returned by GetCurrentCPUArchitecture() matches the active macro, and the
// IsARM()/IsARMHF()/IsARMEL() predicates agree with it.

#ifdef CONTENT_NW_SRC_API_BINDINGS_COMMON_UNITTEST
#include <gtest/gtest.h>

namespace {

TEST(BindingsCommonArchTest, ReportsConsistentArchitecture) {
  const std::string arch = GetCurrentCPUArchitecture();
  EXPECT_FALSE(arch.empty());
  EXPECT_EQ(IsARM(), arch == "armel" || arch == "armhf" || arch == "arm64");
  EXPECT_EQ(IsARMHF(), arch == "armhf");
  EXPECT_EQ(IsARMEL(), arch == "armel");
}

TEST(BindingsCommonArchTest, MacrosAreMutuallyExclusive) {
  int count = 0;
#ifdef BINDINGS_ARCH_ARMEL
  ++count;
#endif
#ifdef BINDINGS_ARCH_ARMHF
  ++count;
#endif
#ifdef BINDINGS_ARCH_ARM64
  ++count;
#endif
#ifdef BINDINGS_ARCH_X86
  ++count;
#endif
#ifdef BINDINGS_ARCH_X64
  ++count;
#endif
#ifdef BINDINGS_ARCH_UNKNOWN
  ++count;
#endif
  EXPECT_EQ(count, 1);
}

}  // namespace
#endif  // CONTENT_NW_SRC_API_BINDINGS_COMMON_UNITTEST

#endif  // CONTENT_NW_SRC_API_BINDINGS_COMMON_H_