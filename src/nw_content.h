#ifndef NWJS_CONTENT_HOOKS_H
#define NWJS_CONTENT_HOOKS_H

#include "nw_package.h"
#include "base/memory/scoped_ptr.h"
#include "third_party/WebKit/public/web/WebNavigationPolicy.h"
#include "third_party/WebKit/public/platform/WebPageVisibilityState.h"

namespace base {
  class DictionaryValue;
  class CommandLine;
}

namespace blink {
  class WebFrame;
  class WebLocalFrame;
  class WebURLRequest;
  class WebString;
}

namespace content {
  class RenderWidget;
  class RenderFrame;
  class RenderFrameHost;
  class RenderProcessHost;
  class RenderViewHost;
  class NotificationDetails;
  class RenderView;
  class WebContents;
  struct WebPreferences;
}

namespace extensions {
  class Dispatcher;
  class Extension;
  class ScriptContext;
  class Dispatcher;
  class AppWindow;
}

namespace nw {
int MainPartsPreCreateThreadsHook();
void MainPartsPostDestroyThreadsHook();
void MainPartsPreMainMessageLoopRunHook();
void ContextCreationHook(blink::WebLocalFrame* frame, extensions::ScriptContext* context);
void LoadNWAppAsExtensionHook(base::DictionaryValue* manifest, std::string* error);
void DocumentElementHook(blink::WebLocalFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url);
void DocumentFinishHook(blink::WebFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url);
 void DocumentHook2(bool start, content::RenderFrame* frame, extensions::Dispatcher* dispatcher);
 void RendererProcessTerminatedHook(content::RenderProcessHost* process,
                                    const content::NotificationDetails& details);
 void OnRenderProcessShutdownHook(extensions::ScriptContext* context);
 void willHandleNavigationPolicy(content::RenderView* rv,
                                 blink::WebFrame* frame,
                                 const blink::WebURLRequest& request,
                                 blink::WebNavigationPolicy* policy,
                                 blink::WebString* manifest,
                                 bool new_win);
 void ExtensionDispatcherCreated(extensions::Dispatcher* dispatcher);
 void CalcNewWinParams(content::WebContents* new_contents, void* params,
                       std::string* nw_inject_js_doc_start,
                       std::string* nw_inject_js_doc_end);
 bool GetImage(Package* package, const FilePath& icon_path, gfx::Image* image);
 scoped_ptr<base::DictionaryValue> MergeManifest();
 bool ExecuteAppCommandHook(int command_id, extensions::AppWindow* app_window);
 bool ProcessSingletonNotificationCallbackHook(const base::CommandLine& command_line,
                                               const base::FilePath& current_directory);
 bool GetUserAgentFromManifest(std::string* agent);
 void SetUserAgentOverride(const std::string& agent,
                           const std::string& name,
                           const std::string& version);

 bool GetPackageImage(nw::Package* package,
                      const base::FilePath& icon_path,
                      gfx::Image* image);
#if defined(OS_MACOSX)
 bool ApplicationShouldHandleReopenHook(bool hasVisibleWindows);
 void OSXOpenURLsHook(const std::vector<GURL>& startup_urls);
#endif
 void CreateAppWindowHook(extensions::AppWindow*);
 void ReloadExtensionHook(const extensions::Extension*);
 bool IsReloadingApp();
 void KickNextTick();
 void OverrideWebkitPrefsHook(content::RenderViewHost* rvh, content::WebPreferences* web_prefs);
 bool PinningRenderer();
 void SetPinningRenderer(bool pin);
 void ShowDevtools(bool show, content::WebContents* web_contents, content::WebContents* container = nullptr);
 bool CheckStoragePartitionMatches(int render_process_id, const GURL& url);
 bool RphGuestFilterURLHook(content::RenderProcessHost* rph, const GURL* url);
 bool ShouldServiceRequestHook(int child_id, const GURL& url);
 bool RenderWidgetWasHiddenHook(content::RenderWidget* rw);
}

#endif
