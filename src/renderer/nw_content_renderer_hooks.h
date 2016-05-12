#ifndef NW_CONTENT_RENDER_HOOKS_H_
#define NW_CONTENT_RENDER_HOOKS_H_

namespace content {
  class RenderWidget;
}

namespace nw {

// renderer
// ref in base/message_loop/message_pumpuv_mac.mm
void KickNextTick();
// ref in content/renderer/in_process_renderer_thread.cc
void LoadNodeSymbols();

//
// implemented in nw_extensions_renderer_hooks.cc
//
// ref in content/renderer/render_widget.cc
bool RenderWidgetWasHiddenHook(content::RenderWidget* rw);


} // nw

#endif // NW_CONTENT_RENDER_HOOKS_H_