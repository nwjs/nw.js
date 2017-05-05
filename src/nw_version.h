// Copyright (c) 2012 Roger Wang <roger.wang@linux.intel.com>
//                    Zhao Cheng <zcbenz@gmail.com>
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

#ifndef NW_VERSION_H
#define NW_VERSION_H

#define NW_MAJOR_VERSION 0
#define NW_MINOR_VERSION 23
#define NW_PATCH_VERSION 0

#define NW_VERSION_IS_RELEASE 1

#ifndef NW_STRINGIFY
#define NW_STRINGIFY(n) NW_STRINGIFY_HELPER(n)
#define NW_STRINGIFY_HELPER(n) #n
#endif

#if NW_VERSION_IS_RELEASE
# define NW_VERSION_STRING  NW_STRINGIFY(NW_MAJOR_VERSION) "." \
                              NW_STRINGIFY(NW_MINOR_VERSION) "." \
                              NW_STRINGIFY(NW_PATCH_VERSION)
#else
# define NW_VERSION_STRING  NW_STRINGIFY(NW_MAJOR_VERSION) "." \
                              NW_STRINGIFY(NW_MINOR_VERSION) "." \
                              NW_STRINGIFY(NW_PATCH_VERSION) "-beta2"
#endif

#define NW_VERSION "v" NW_VERSION_STRING

#define NW_VERSION_AT_LEAST(major, minor, patch) \
  (( (major) < NW_MAJOR_VERSION) \
  || ((major) == NW_MAJOR_VERSION && (minor) < NW_MINOR_VERSION) \
  || ((major) == NW_MAJOR_VERSION && (minor) == NW_MINOR_VERSION && (patch) <= NW_PATCH_VERSION))

#endif /* NW_VERSION_H */
