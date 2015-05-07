#ifndef NW_EXPORT_H_
#define NW_EXPORT_H_

#if defined(COMPONENT_BUILD) && !defined(COMPILE_CONTENT_STATICALLY)
#if defined(WIN32)

#if defined(NW_IMPLEMENTATION)
#define NW_EXPORT __declspec(dllexport)
#else
#define NW_EXPORT __declspec(dllimport)
#endif  // defined(CONTENT_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(NW_IMPLEMENTATION)
#define NW_EXPORT __attribute__((visibility("default")))
#else
#define NW_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define NW_EXPORT
#endif

#endif
