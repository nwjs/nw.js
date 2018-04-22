#ifndef NW_CHROME_RENDERER_HOOKS_H_
#define NW_CHROME_RENDERER_HOOKS_H_

#include "third_party/blink/public/web/web_navigation_policy.h"

namespace blink {
  class WebFrame;
  class WebString;
  class WebURLRequest;
}

namespace content {
  class RenderView;
}

namespace extensions {
  class Dispatcher;
}

namespace nw {
//
// implemented in nw_extensions_renderer_hooks.cc
//
// ref in chrome/renderer/chrome_content_renderer_client.cc
void willHandleNavigationPolicy(content::RenderView* rv,
                               blink::WebFrame* frame,
                               const blink::WebURLRequest& request,
                               blink::WebNavigationPolicy* policy,
                               blink::WebString* manifest,
                               bool new_win);
// ref in chrome/renderer/extensions/chrome_extensions_renderer_client.cc
void ExtensionDispatcherCreated(extensions::Dispatcher* dispatcher);
const std::string& get_main_extension_id();
}

#endif // NW_CHROME_RENDERER_HOOKS_H_
