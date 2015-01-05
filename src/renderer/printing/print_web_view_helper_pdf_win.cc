// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/printing/print_web_view_helper.h"

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/process/process_handle.h"
#include "chrome/common/print_messages.h"
#include "content/public/renderer/render_thread.h"
#include "printing/metafile.h"
#include "printing/metafile_impl.h"
#include "printing/metafile_skia_wrapper.h"
#include "printing/page_size_margins.h"
#include "printing/units.h"
#include "skia/ext/platform_device.h"
#include "skia/ext/vector_canvas.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"


namespace printing {

using blink::WebFrame;

bool PrintWebViewHelper::RenderPreviewPage(
    int page_number,
    const PrintMsg_Print_Params& print_params) {
  PrintMsg_PrintPage_Params page_params;
  page_params.params = print_params;
  page_params.page_number = page_number;
  scoped_ptr<Metafile> draft_metafile;
  Metafile* initial_render_metafile = print_preview_context_.metafile();
  if (print_preview_context_.IsModifiable() && is_print_ready_metafile_sent_) {
    draft_metafile.reset(new PreviewMetafile);
    initial_render_metafile = draft_metafile.get();
  }

  base::TimeTicks begin_time = base::TimeTicks::Now();
  PrintPageInternal(page_params,
                    print_preview_context_.GetPrintCanvasSize(),
                    print_preview_context_.prepared_frame(),
                    initial_render_metafile,
                    NULL,
                    NULL);
  print_preview_context_.RenderedPreviewPage(
      base::TimeTicks::Now() - begin_time);
  if (draft_metafile.get()) {
    draft_metafile->FinishDocument();
  } else if (print_preview_context_.IsModifiable() &&
             print_preview_context_.generate_draft_pages()) {
    DCHECK(!draft_metafile.get());
    draft_metafile.reset(
        print_preview_context_.metafile()->GetMetafileForCurrentPage());
  }
  return PreviewPageRendered(page_number, draft_metafile.get());
}

bool PrintWebViewHelper::PrintPagesNative(blink::WebFrame* frame,
                                          int page_count,
                                          const gfx::Size& canvas_size) {
  NativeMetafile metafile;
  if (!metafile.Init())
    return false;

  const PrintMsg_PrintPages_Params& params = *print_pages_params_;
  std::vector<int> printed_pages;
  if (params.pages.empty()) {
    for (int i = 0; i < page_count; ++i) {
      printed_pages.push_back(i);
    }
  } else {
    // TODO(vitalybuka): redesign to make more code cross platform.
    for (size_t i = 0; i < params.pages.size(); ++i) {
      if (params.pages[i] >= 0 && params.pages[i] < page_count) {
        printed_pages.push_back(params.pages[i]);
      }
    }
  }
  if (printed_pages.empty())
    return false;

  std::vector<gfx::Size> page_size_in_dpi(printed_pages.size());
  std::vector<gfx::Rect> content_area_in_dpi(printed_pages.size());

  PrintMsg_PrintPage_Params page_params;
  page_params.params = params.params;
  for (size_t i = 0; i < printed_pages.size(); ++i) {
    page_params.page_number = printed_pages[i];
    PrintPageInternal(page_params,
                      canvas_size,
                      frame,
                      &metafile,
                      &page_size_in_dpi[i],
                      &content_area_in_dpi[i]);
  }

  // blink::printEnd() for PDF should be called before metafile is closed.
  FinishFramePrinting();

  metafile.FinishDocument();

  // Get the size of the resulting metafile.
  uint32 buf_size = metafile.GetDataSize();
  DCHECK_GT(buf_size, 0u);

  PrintHostMsg_DidPrintPage_Params printed_page_params;
  printed_page_params.data_size = 0;
  printed_page_params.document_cookie = params.params.document_cookie;
  printed_page_params.page_size = params.params.page_size;
  printed_page_params.content_area = params.params.printable_area;

  {
    base::SharedMemory shared_buf;
    // Allocate a shared memory buffer to hold the generated metafile data.
    if (!shared_buf.CreateAndMapAnonymous(buf_size)) {
      NOTREACHED() << "Buffer allocation failed";
      return false;
    }

    // Copy the bits into shared memory.
    if (!metafile.GetData(shared_buf.memory(), buf_size)) {
      NOTREACHED() << "GetData() failed";
      shared_buf.Unmap();
      return false;
    }
    shared_buf.GiveToProcess(base::GetCurrentProcessHandle(),
                             &printed_page_params.metafile_data_handle);
    shared_buf.Unmap();

    printed_page_params.data_size = buf_size;
    Send(new PrintHostMsg_DuplicateSection(
        routing_id(),
        printed_page_params.metafile_data_handle,
        &printed_page_params.metafile_data_handle));
  }

  for (size_t i = 0; i < printed_pages.size(); ++i) {
    printed_page_params.page_number = printed_pages[i];
    printed_page_params.page_size = page_size_in_dpi[i];
    printed_page_params.content_area = content_area_in_dpi[i];
    Send(new PrintHostMsg_DidPrintPage(routing_id(), printed_page_params));
    printed_page_params.metafile_data_handle = INVALID_HANDLE_VALUE;
  }
  return true;
}

void PrintWebViewHelper::PrintPageInternal(
    const PrintMsg_PrintPage_Params& params,
    const gfx::Size& canvas_size,
    WebFrame* frame,
    Metafile* metafile,
    gfx::Size* page_size_in_dpi,
    gfx::Rect* content_area_in_dpi) {
  PageSizeMargins page_layout_in_points;
  double css_scale_factor = 1.0f;
  ComputePageLayoutInPointsForCss(frame, params.page_number, params.params,
                                  ignore_css_margins_, &css_scale_factor,
                                  &page_layout_in_points);
  gfx::Size page_size;
  gfx::Rect content_area;
  GetPageSizeAndContentAreaFromPageLayout(page_layout_in_points, &page_size,
                                          &content_area);
  int dpi = static_cast<int>(params.params.dpi);
  // Calculate the actual page size and content area in dpi.
  if (page_size_in_dpi) {
    *page_size_in_dpi =
        gfx::Size(static_cast<int>(ConvertUnitDouble(
                      page_size.width(), kPointsPerInch, dpi)),
                  static_cast<int>(ConvertUnitDouble(
                      page_size.height(), kPointsPerInch, dpi)));
  }

  if (content_area_in_dpi) {
    // Output PDF matches paper size and should be printer edge to edge.
    *content_area_in_dpi =
        gfx::Rect(0, 0, page_size_in_dpi->width(), page_size_in_dpi->height());
  }

  gfx::Rect canvas_area =
      params.params.display_header_footer ? gfx::Rect(page_size) : content_area;

  float webkit_page_shrink_factor =
      frame->getPrintPageShrink(params.page_number);
  float scale_factor = css_scale_factor * webkit_page_shrink_factor;

  SkBaseDevice* device = metafile->StartPageForVectorCanvas(page_size,
                                                            canvas_area,
                                                            scale_factor);
  if (!device)
    return;

  // The printPage method take a reference to the canvas we pass down, so it
  // can't be a stack object.
  skia::RefPtr<skia::VectorCanvas> canvas =
      skia::AdoptRef(new skia::VectorCanvas(device));
  MetafileSkiaWrapper::SetMetafileOnCanvas(*canvas, metafile);
  skia::SetIsDraftMode(*canvas, is_print_ready_metafile_sent_);

  if (params.params.display_header_footer) {
    // |page_number| is 0-based, so 1 is added.
    PrintHeaderAndFooter(canvas.get(),
                         params.page_number + 1,
                         print_preview_context_.total_page_count(),
                         scale_factor,
                         page_layout_in_points,
                         *header_footer_info_,
                         params.params);
  }

  float webkit_scale_factor = RenderPageContent(frame,
                                                params.page_number,
                                                canvas_area,
                                                content_area,
                                                scale_factor,
                                                canvas.get());
  DCHECK_GT(webkit_scale_factor, 0.0f);
  // Done printing. Close the device context to retrieve the compiled metafile.
  if (!metafile->FinishPage())
    NOTREACHED() << "metafile failed";
}

bool PrintWebViewHelper::CopyMetafileDataToSharedMem(
    Metafile* metafile, base::SharedMemoryHandle* shared_mem_handle) {
  uint32 buf_size = metafile->GetDataSize();
  base::SharedMemory shared_buf;
  // Allocate a shared memory buffer to hold the generated metafile data.
  if (!shared_buf.CreateAndMapAnonymous(buf_size)) {
    NOTREACHED() << "Buffer allocation failed";
    return false;
  }

  // Copy the bits into shared memory.
  if (!metafile->GetData(shared_buf.memory(), buf_size)) {
    NOTREACHED() << "GetData() failed";
    shared_buf.Unmap();
    return false;
  }
  shared_buf.GiveToProcess(base::GetCurrentProcessHandle(), shared_mem_handle);
  shared_buf.Unmap();

  Send(new PrintHostMsg_DuplicateSection(routing_id(), *shared_mem_handle,
                                         shared_mem_handle));
  return true;
}

}  // namespace printing
