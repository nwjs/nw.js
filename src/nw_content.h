#ifndef NWJS_CONTENT_HOOKS_H
#define NWJS_CONTENT_HOOKS_H

#include "nw_package.h"

namespace base {
  class DictionaryValue;
}

namespace blink {
  class WebFrame;
}

namespace extensions {
  class Extension;
  class ScriptContext;
}

namespace nw {
int MainPartsPreCreateThreadsHook();
void MainPartsPostDestroyThreadsHook();
void ContextCreationHook(extensions::ScriptContext* context);
void LoadNWAppAsExtensionHook(base::DictionaryValue* manifest, std::string* error);
void DocumentElementHook(blink::WebFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url);
}

#endif
