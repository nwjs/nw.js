// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_RENDERER_PRINTING_PRINT_WEB_VIEW_HELPER_H_
#define CHROME_RENDERER_PRINTING_PRINT_WEB_VIEW_HELPER_H_

#include <vector>

#include "base/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/shared_memory.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "content/public/renderer/render_view_observer.h"
#include "content/public/renderer/render_view_observer_tracker.h"
#include "printing/pdf_metafile_skia.h"
#include "third_party/WebKit/public/platform/WebCanvas.h"
#include "third_party/WebKit/public/web/WebNode.h"
#include "third_party/WebKit/public/web/WebPrintParams.h"
#include "ui/gfx/geometry/size.h"

struct PrintMsg_Print_Params;
struct PrintMsg_PrintPage_Params;
struct PrintMsg_PrintPages_Params;
struct PrintHostMsg_SetOptionsFromDocument_Params;

namespace base {
class DictionaryValue;
}

namespace blink {
class WebFrame;
class WebView;
}

namespace printing {

struct PageSizeMargins;
class PrepareFrameAndViewForPrint;

// Stores reference to frame using WebVew and unique name.
// Workaround to modal dialog issue on Linux. crbug.com/236147.
// If WebFrame someday supports WeakPtr, we should use it here.
class FrameReference {
 public:
  explicit FrameReference(blink::WebLocalFrame* frame);
  FrameReference();
  ~FrameReference();

  void Reset(blink::WebLocalFrame* frame);

  blink::WebLocalFrame* GetFrame();
  blink::WebView* view();

 private:
  blink::WebView* view_;
  blink::WebLocalFrame* frame_;
};

// PrintWebViewHelper handles most of the printing grunt work for RenderView.
// We plan on making print asynchronous and that will require copying the DOM
// of the document and creating a new WebView with the contents.
class PrintWebViewHelper
    : public content::RenderViewObserver,
      public content::RenderViewObserverTracker<PrintWebViewHelper> {
 public:

  class Delegate {
   public:
    virtual ~Delegate() {}

    // Cancels prerender if it's currently in progress and returns |true| if
    // the cancellation was done with success.
    virtual bool CancelPrerender(content::RenderView* render_view,
                                 int routing_id) = 0;

    // Returns the element to be printed. Returns a null WebElement if
    // a pdf plugin element can't be extracted from the frame.
    virtual blink::WebElement GetPdfElement(blink::WebLocalFrame* frame) = 0;
  };

  PrintWebViewHelper(content::RenderView* render_view,
                     bool out_of_process_pdf_enabled,
                     bool print_preview_disabled,
                     scoped_ptr<Delegate> delegate);
  ~PrintWebViewHelper() override;

  // Disable print preview and switch to system dialog printing even if full
  // printing is build-in. This method is used by CEF.
  static void DisablePreview();

  bool IsPrintingEnabled();

  void PrintNode(const blink::WebNode& node);

 private:
  friend class PrintWebViewHelperTestBase;
  FRIEND_TEST_ALL_PREFIXES(PrintWebViewHelperPreviewTest,
                           BlockScriptInitiatedPrinting);
  FRIEND_TEST_ALL_PREFIXES(PrintWebViewHelperTest, OnPrintPages);
  FRIEND_TEST_ALL_PREFIXES(PrintWebViewHelperTest,
                           BlockScriptInitiatedPrinting);
  FRIEND_TEST_ALL_PREFIXES(PrintWebViewHelperTest,
                           BlockScriptInitiatedPrintingFromPopup);
#if defined(OS_WIN) || defined(OS_MACOSX)
  FRIEND_TEST_ALL_PREFIXES(PrintWebViewHelperTest, PrintLayoutTest);
  FRIEND_TEST_ALL_PREFIXES(PrintWebViewHelperTest, PrintWithIframe);
#endif  // defined(OS_WIN) || defined(OS_MACOSX)

  enum PrintingResult {
    OK,
    FAIL_PRINT_INIT,
    FAIL_PRINT,
    FAIL_PREVIEW,
  };

  enum PrintPreviewErrorBuckets {
    PREVIEW_ERROR_NONE,  // Always first.
    PREVIEW_ERROR_BAD_SETTING,
    PREVIEW_ERROR_METAFILE_COPY_FAILED,
    PREVIEW_ERROR_METAFILE_INIT_FAILED,
    PREVIEW_ERROR_ZERO_PAGES,
    PREVIEW_ERROR_MAC_DRAFT_METAFILE_INIT_FAILED,
    PREVIEW_ERROR_PAGE_RENDERED_WITHOUT_METAFILE,
    PREVIEW_ERROR_INVALID_PRINTER_SETTINGS,
    PREVIEW_ERROR_LAST_ENUM  // Always last.
  };

  enum PrintPreviewRequestType {
    PRINT_PREVIEW_USER_INITIATED_ENTIRE_FRAME,
    PRINT_PREVIEW_USER_INITIATED_SELECTION,
    PRINT_PREVIEW_USER_INITIATED_CONTEXT_NODE,
    PRINT_PREVIEW_SCRIPTED  // triggered by window.print().
  };

  // RenderViewObserver implementation.
  bool OnMessageReceived(const IPC::Message& message) override;
  void PrintPage(blink::WebLocalFrame* frame, bool user_initiated) override;
  void DidStartLoading() override;
  void DidStopLoading() override;

  // Message handlers ---------------------------------------------------------
#if defined(ENABLE_BASIC_PRINTING)
  void OnPrintPages();
  void OnPrintForSystemDialog();
#endif  // ENABLE_BASIC_PRINTING
  void OnInitiatePrintPreview(bool selection_only);
  void OnPrintPreview(const base::DictionaryValue& settings);
  void OnPrintForPrintPreview(const base::DictionaryValue& job_settings);
  void OnPrintingDone(bool success);

  // Get |page_size| and |content_area| information from
  // |page_layout_in_points|.
  void GetPageSizeAndContentAreaFromPageLayout(
      const PageSizeMargins& page_layout_in_points,
      gfx::Size* page_size,
      gfx::Rect* content_area);

  // Update |ignore_css_margins_| based on settings.
  void UpdateFrameMarginsCssInfo(const base::DictionaryValue& settings);

  // Returns true if the current destination printer is PRINT_TO_PDF.
  bool IsPrintToPdfRequested(const base::DictionaryValue& settings);

  // Prepare frame for creating preview document.
  void PrepareFrameForPreviewDocument();

  // Continue creating preview document.
  void OnFramePreparedForPreviewDocument();

  // Initialize the print preview document.
  bool CreatePreviewDocument();

  // Renders a print preview page. |page_number| is 0-based.
  // Returns true if print preview should continue, false on failure.
  bool RenderPreviewPage(int page_number,
                         const PrintMsg_Print_Params& print_params);

  // Finalize the print ready preview document.
  bool FinalizePrintReadyDocument();

  // Enable/Disable window.print calls.  If |blocked| is true window.print
  // calls will silently fail.  Call with |blocked| set to false to reenable.
  void SetScriptedPrintBlocked(bool blocked);

  // Main printing code -------------------------------------------------------

  // |is_scripted| should be true when the call is coming from window.print()
  void Print(blink::WebLocalFrame* frame,
             const blink::WebNode& node,
             bool is_scripted);

  // Notification when printing is done - signal tear-down/free resources.
  void DidFinishPrinting(PrintingResult result);

  // Print Settings -----------------------------------------------------------

  // Initialize print page settings with default settings.
  // Used only for native printing workflow.
  bool InitPrintSettings(bool fit_to_paper_size);

  // Calculate number of pages in source document.
  bool CalculateNumberOfPages(blink::WebLocalFrame* frame,
                              const blink::WebNode& node,
                              int* number_of_pages);

  // Set options for print preset from source PDF document.
  void SetOptionsFromDocument(
      PrintHostMsg_SetOptionsFromDocument_Params& params);

  // Update the current print settings with new |passed_job_settings|.
  // |passed_job_settings| dictionary contains print job details such as printer
  // name, number of copies, page range, etc.
  bool UpdatePrintSettings(blink::WebLocalFrame* frame,
                           const blink::WebNode& node,
                           const base::DictionaryValue& passed_job_settings);

  // Get final print settings from the user.
  // Return false if the user cancels or on error.
  bool GetPrintSettingsFromUser(blink::WebFrame* frame,
                                const blink::WebNode& node,
                                int expected_pages_count,
                                bool is_scripted);

  // Page Printing / Rendering ------------------------------------------------

  void OnFramePreparedForPrintPages();
  void PrintPages();
  bool PrintPagesNative(blink::WebFrame* frame, int page_count);
  void FinishFramePrinting();

  // Prints the page listed in |params|.
#if defined(OS_LINUX) || defined(OS_ANDROID)
  void PrintPageInternal(const PrintMsg_PrintPage_Params& params,
                         blink::WebFrame* frame,
                         PdfMetafileSkia* metafile);
#elif defined(OS_WIN)
  void PrintPageInternal(const PrintMsg_PrintPage_Params& params,
                         blink::WebFrame* frame,
                         PdfMetafileSkia* metafile,
                         gfx::Size* page_size_in_dpi,
                         gfx::Rect* content_area_in_dpi);
#else
  void PrintPageInternal(const PrintMsg_PrintPage_Params& params,
                         blink::WebFrame* frame);
#endif

  // Render the frame for printing.
  bool RenderPagesForPrint(blink::WebLocalFrame* frame,
                           const blink::WebNode& node);

  // Platform specific helper function for rendering page(s) to |metafile|.
#if defined(OS_MACOSX)
  void RenderPage(const PrintMsg_Print_Params& params,
                  int page_number,
                  blink::WebFrame* frame,
                  bool is_preview,
                  PdfMetafileSkia* metafile,
                  gfx::Size* page_size,
                  gfx::Rect* content_rect);
#endif  // defined(OS_MACOSX)

  // Renders page contents from |frame| to |content_area| of |canvas|.
  // |page_number| is zero-based.
  // When method is called, canvas should be setup to draw to |canvas_area|
  // with |scale_factor|.
  static float RenderPageContent(blink::WebFrame* frame,
                                 int page_number,
                                 const gfx::Rect& canvas_area,
                                 const gfx::Rect& content_area,
                                 double scale_factor,
                                 blink::WebCanvas* canvas);

  // Helper methods -----------------------------------------------------------

  bool CopyMetafileDataToSharedMem(PdfMetafileSkia* metafile,
                                   base::SharedMemoryHandle* shared_mem_handle);

  // Helper method to get page layout in points and fit to page if needed.
  static void ComputePageLayoutInPointsForCss(
      blink::WebFrame* frame,
      int page_index,
      const PrintMsg_Print_Params& default_params,
      bool ignore_css_margins,
      double* scale_factor,
      PageSizeMargins* page_layout_in_points);

#if defined(ENABLE_PRINT_PREVIEW)
  // Given the |device| and |canvas| to draw on, prints the appropriate headers
  // and footers using strings from |header_footer_info| on to the canvas.
  static void PrintHeaderAndFooter(blink::WebCanvas* canvas,
                                   int page_number,
                                   int total_pages,
                                   const blink::WebFrame& source_frame,
                                   float webkit_scale_factor,
                                   const PageSizeMargins& page_layout_in_points,
                                   const PrintMsg_Print_Params& params);
#endif  // defined(ENABLE_PRINT_PREVIEW)

  bool GetPrintFrame(blink::WebLocalFrame** frame);

  // Script Initiated Printing ------------------------------------------------

  // Return true if script initiated printing is currently
  // allowed. |user_initiated| should be true when a user event triggered the
  // script, most likely by pressing a print button on the page.
  bool IsScriptInitiatedPrintAllowed(blink::WebFrame* frame,
                                     bool user_initiated);

  // Shows scripted print preview when options from plugin are available.
  void ShowScriptedPrintPreview();

  void RequestPrintPreview(PrintPreviewRequestType type);

  // Checks whether print preview should continue or not.
  // Returns true if canceling, false if continuing.
  bool CheckForCancel();

  // Notifies the browser a print preview page has been rendered.
  // |page_number| is 0-based.
  // For a valid |page_number| with modifiable content,
  // |metafile| is the rendered page. Otherwise |metafile| is NULL.
  // Returns true if print preview should continue, false on failure.
  bool PreviewPageRendered(int page_number, PdfMetafileSkia* metafile);

  void SetPrintPagesParams(const PrintMsg_PrintPages_Params& settings);

  // WebView used only to print the selection.
  scoped_ptr<PrepareFrameAndViewForPrint> prep_frame_view_;
  bool reset_prep_frame_view_;

  scoped_ptr<PrintMsg_PrintPages_Params> print_pages_params_;
  bool is_print_ready_metafile_sent_;
  bool ignore_css_margins_;

  // Used for scripted initiated printing blocking.
  bool is_scripted_printing_blocked_;

  // Let the browser process know of a printing failure. Only set to false when
  // the failure came from the browser in the first place.
  bool notify_browser_of_print_failure_;

  // True, when printing from print preview.
  bool print_for_preview_;

  // Whether the content to print could be nested in an iframe.
  const bool out_of_process_pdf_enabled_;

  // Used to check the prerendering status.
  const scoped_ptr<Delegate> delegate_;

  // Keeps track of the state of print preview between messages.
  // TODO(vitalybuka): Create PrintPreviewContext when needed and delete after
  // use. Now it's interaction with various messages is confusing.
  class PrintPreviewContext {
   public:
    PrintPreviewContext();
    ~PrintPreviewContext();

    // Initializes the print preview context. Need to be called to set
    // the |web_frame| / |web_node| to generate the print preview for.
    void InitWithFrame(blink::WebLocalFrame* web_frame);
    void InitWithNode(const blink::WebNode& web_node);

    // Does bookkeeping at the beginning of print preview.
    void OnPrintPreview();

    // Create the print preview document. |pages| is empty to print all pages.
    // Takes ownership of |prepared_frame|.
    bool CreatePreviewDocument(PrepareFrameAndViewForPrint* prepared_frame,
                               const std::vector<int>& pages);

    // Called after a page gets rendered. |page_time| is how long the
    // rendering took.
    void RenderedPreviewPage(const base::TimeDelta& page_time);

    // Updates the print preview context when the required pages are rendered.
    void AllPagesRendered();

    // Finalizes the print ready preview document.
    void FinalizePrintReadyDocument();

    // Cleanup after print preview finishes.
    void Finished();

    // Cleanup after print preview fails.
    void Failed(bool report_error);

    // Helper functions
    int GetNextPageNumber();
    bool IsRendering() const;
    bool IsModifiable();
    bool HasSelection();
    bool IsLastPageOfPrintReadyMetafile() const;
    bool IsFinalPageRendered() const;

    // Setters
    void set_generate_draft_pages(bool generate_draft_pages);
    void set_error(enum PrintPreviewErrorBuckets error);

    // Getters
    // Original frame for which preview was requested.
    blink::WebLocalFrame* source_frame();
    // Original node for which preview was requested.
    const blink::WebNode& source_node() const;

    // Frame to be use to render preview. May be the same as source_frame(), or
    // generated from it, e.g. copy of selected block.
    blink::WebLocalFrame* prepared_frame();
    // Node to be use to render preview. May be the same as source_node(), or
    // generated from it, e.g. copy of selected block.
    const blink::WebNode& prepared_node() const;

    int total_page_count() const;
    bool generate_draft_pages() const;
    PdfMetafileSkia* metafile();
    int last_error() const;

   private:
    enum State {
      UNINITIALIZED,  // Not ready to render.
      INITIALIZED,    // Ready to render.
      RENDERING,      // Rendering.
      DONE            // Finished rendering.
    };

    // Reset some of the internal rendering context.
    void ClearContext();

    // Specifies what to render for print preview.
    FrameReference source_frame_;
    blink::WebNode source_node_;

    scoped_ptr<PrepareFrameAndViewForPrint> prep_frame_view_;
    scoped_ptr<PdfMetafileSkia> metafile_;

    // Total page count in the renderer.
    int total_page_count_;

    // The current page to render.
    int current_page_index_;

    // List of page indices that need to be rendered.
    std::vector<int> pages_to_render_;

    // True, when draft pages needs to be generated.
    bool generate_draft_pages_;

    // Specifies the total number of pages in the print ready metafile.
    int print_ready_metafile_page_count_;

    base::TimeDelta document_render_time_;
    base::TimeTicks begin_time_;

    enum PrintPreviewErrorBuckets error_;

    State state_;
  };

  class ScriptingThrottler {
   public:
    ScriptingThrottler();

    // Returns false if script initiated printing occurs too often.
    bool IsAllowed(blink::WebFrame* frame);

    // Reset the counter for script initiated printing.
    // Scripted printing will be allowed to continue.
    void Reset();

   private:
    base::Time last_print_;
    int count_ = 0;
    DISALLOW_COPY_AND_ASSIGN(ScriptingThrottler);
  };

  ScriptingThrottler scripting_throttler_;

  bool print_node_in_progress_;
  PrintPreviewContext print_preview_context_;
  bool is_loading_;
  bool is_scripted_preview_delayed_;

  // Used to fix a race condition where the source is a PDF and print preview
  // hangs because RequestPrintPreview is called before DidStopLoading() is
  // called. This is a store for the RequestPrintPreview() call and its
  // parameters so that it can be invoked after DidStopLoading.
  base::Closure on_stop_loading_closure_;

  base::WeakPtrFactory<PrintWebViewHelper> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(PrintWebViewHelper);
};

}  // namespace printing

#endif  // CHROME_RENDERER_PRINTING_PRINT_WEB_VIEW_HELPER_H_
