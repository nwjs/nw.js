#ifndef NW_EXTENSIONS_RENDERER_HOOKS_H_
#define NW_EXTENSIONS_RENDERER_HOOKS_H_

#include <string>

class GURL;

namespace blink {
  class WebFrame;
  class WebLocalFrame;
}

namespace content {
  class RenderFrame;
}

namespace extensions {
  class Dispatcher;
  class Extension;
  class ScriptContext;
}

namespace nw {
// renderer
// ref in extensions/renderer/dispatcher.cc
void ContextCreationHook(blink::WebLocalFrame* frame, extensions::ScriptContext* context);
// ref in extensions/renderer/dispatcher.cc
void DocumentElementHook(blink::WebLocalFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url);
// ref in extensions/renderer/dispatcher.cc
void DocumentFinishHook(blink::WebLocalFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url);
// ref in extensions/renderer/extension_frame_helper.cc
void DocumentHook2(bool start, content::RenderFrame* frame, extensions::Dispatcher* dispatcher);
// ref in extensions/renderer/dispatcher.cc
void OnRenderProcessShutdownHook(extensions::ScriptContext* context);
}

#endif // NW_EXTENSIONS_RENDERER_HOOKS_H_
