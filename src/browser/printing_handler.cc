// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/utility/printing_handler.h"

#include "base/file_util.h"
#include "base/lazy_instance.h"
#include "base/path_service.h"
#include "base/scoped_native_library.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_utility_printing_messages.h"
#include "chrome/utility/cloud_print/bitmap_image.h"
#include "chrome/utility/cloud_print/pwg_encoder.h"
#include "content/public/utility/utility_thread.h"
#include "printing/page_range.h"
#include "printing/pdf_render_settings.h"

#if defined(OS_WIN)
#include "base/win/iat_patch_function.h"
#include "printing/emf_win.h"
#include "ui/gfx/gdi_util.h"
#endif

#if defined(ENABLE_FULL_PRINTING)
#include "chrome/common/crash_keys.h"
#include "printing/backend/print_backend.h"
#endif

namespace {
const base::FilePath::CharType kInternalPDFPluginFileName[] =
#if defined(OS_WIN)
    FILE_PATH_LITERAL("pdf.dll");
#elif defined(OS_MACOSX)
    FILE_PATH_LITERAL("PDF.plugin");
#else  // Linux and Chrome OS
    FILE_PATH_LITERAL("libpdf.so");
#endif

bool Send(IPC::Message* message) {
  return content::UtilityThread::Get()->Send(message);
}

void ReleaseProcessIfNeeded() {
  content::UtilityThread::Get()->ReleaseProcessIfNeeded();
}

class PdfFunctionsBase {
 public:
  PdfFunctionsBase() : render_pdf_to_bitmap_func_(NULL),
                       get_pdf_doc_info_func_(NULL) {}

  bool Init() {
    base::FilePath pdf_module_path;
    if (!PathService::Get(base::DIR_MODULE, &pdf_module_path))
      return false;
    pdf_module_path = pdf_module_path.Append(kInternalPDFPluginFileName);
    if (!base::PathExists(pdf_module_path)) {
      return false;
    }

    pdf_lib_.Reset(base::LoadNativeLibrary(pdf_module_path, NULL));
    if (!pdf_lib_.is_valid()) {
      LOG(WARNING) << "Couldn't load PDF plugin";
      return false;
    }

    render_pdf_to_bitmap_func_ =
        reinterpret_cast<RenderPDFPageToBitmapProc>(
            pdf_lib_.GetFunctionPointer("RenderPDFPageToBitmap"));
    LOG_IF(WARNING, !render_pdf_to_bitmap_func_) <<
        "Missing RenderPDFPageToBitmap";

    get_pdf_doc_info_func_ =
        reinterpret_cast<GetPDFDocInfoProc>(
            pdf_lib_.GetFunctionPointer("GetPDFDocInfo"));
    LOG_IF(WARNING, !get_pdf_doc_info_func_) << "Missing GetPDFDocInfo";

    if (!render_pdf_to_bitmap_func_ || !get_pdf_doc_info_func_ ||
        !PlatformInit(pdf_module_path, pdf_lib_)) {
      Reset();
    }

    return IsValid();
  }

  bool IsValid() const {
    return pdf_lib_.is_valid();
  }

  void Reset() {
    pdf_lib_.Reset(NULL);
  }

  bool RenderPDFPageToBitmap(const void* pdf_buffer,
                             int pdf_buffer_size,
                             int page_number,
                             void* bitmap_buffer,
                             int bitmap_width,
                             int bitmap_height,
                             int dpi_x,
                             int dpi_y,
                             bool autorotate) {
    if (!render_pdf_to_bitmap_func_)
      return false;
    return render_pdf_to_bitmap_func_(pdf_buffer, pdf_buffer_size, page_number,
                                      bitmap_buffer, bitmap_width,
                                      bitmap_height, dpi_x, dpi_y, autorotate);
  }

  bool GetPDFDocInfo(const void* pdf_buffer,
                     int buffer_size,
                     int* page_count,
                     double* max_page_width) {
    if (!get_pdf_doc_info_func_)
      return false;
    return get_pdf_doc_info_func_(pdf_buffer, buffer_size, page_count,
                                  max_page_width);
  }

 protected:
  virtual bool PlatformInit(
      const base::FilePath& pdf_module_path,
      const base::ScopedNativeLibrary& pdf_lib) {
    return true;
  }

 private:
  // Exported by PDF plugin.
  typedef bool (*RenderPDFPageToBitmapProc)(const void* pdf_buffer,
                                            int pdf_buffer_size,
                                            int page_number,
                                            void* bitmap_buffer,
                                            int bitmap_width,
                                            int bitmap_height,
                                            int dpi_x,
                                            int dpi_y,
                                            bool autorotate);
  typedef bool (*GetPDFDocInfoProc)(const void* pdf_buffer,
                                    int buffer_size, int* page_count,
                                    double* max_page_width);

  RenderPDFPageToBitmapProc render_pdf_to_bitmap_func_;
  GetPDFDocInfoProc get_pdf_doc_info_func_;

  base::ScopedNativeLibrary pdf_lib_;
  DISALLOW_COPY_AND_ASSIGN(PdfFunctionsBase);
};

#if defined(OS_WIN)
// The 2 below IAT patch functions are almost identical to the code in
// render_process_impl.cc. This is needed to work around specific Windows APIs
// used by the Chrome PDF plugin that will fail in the sandbox.
static base::win::IATPatchFunction g_iat_patch_createdca;
HDC WINAPI UtilityProcess_CreateDCAPatch(LPCSTR driver_name,
                                         LPCSTR device_name,
                                         LPCSTR output,
                                         const DEVMODEA* init_data) {
  if (driver_name && (std::string("DISPLAY") == driver_name)) {
    // CreateDC fails behind the sandbox, but not CreateCompatibleDC.
    return CreateCompatibleDC(NULL);
  }

  NOTREACHED();
  return CreateDCA(driver_name, device_name, output, init_data);
}

static base::win::IATPatchFunction g_iat_patch_get_font_data;
DWORD WINAPI UtilityProcess_GetFontDataPatch(
    HDC hdc, DWORD table, DWORD offset, LPVOID buffer, DWORD length) {
  int rv = GetFontData(hdc, table, offset, buffer, length);
  if (rv == GDI_ERROR && hdc) {
    HFONT font = static_cast<HFONT>(GetCurrentObject(hdc, OBJ_FONT));

    LOGFONT logfont;
    if (GetObject(font, sizeof(LOGFONT), &logfont)) {
      content::UtilityThread::Get()->PreCacheFont(logfont);
      rv = GetFontData(hdc, table, offset, buffer, length);
      content::UtilityThread::Get()->ReleaseCachedFonts();
    }
  }
  return rv;
}

class PdfFunctionsWin : public PdfFunctionsBase {
 public:
  PdfFunctionsWin() : render_pdf_to_dc_func_(NULL) {
  }

  bool PlatformInit(
      const base::FilePath& pdf_module_path,
      const base::ScopedNativeLibrary& pdf_lib) OVERRIDE {
    // Patch the IAT for handling specific APIs known to fail in the sandbox.
    if (!g_iat_patch_createdca.is_patched()) {
      g_iat_patch_createdca.Patch(pdf_module_path.value().c_str(),
                                  "gdi32.dll", "CreateDCA",
                                  UtilityProcess_CreateDCAPatch);
    }

    if (!g_iat_patch_get_font_data.is_patched()) {
      g_iat_patch_get_font_data.Patch(pdf_module_path.value().c_str(),
                                      "gdi32.dll", "GetFontData",
                                      UtilityProcess_GetFontDataPatch);
    }
    render_pdf_to_dc_func_ =
      reinterpret_cast<RenderPDFPageToDCProc>(
          pdf_lib.GetFunctionPointer("RenderPDFPageToDC"));
    LOG_IF(WARNING, !render_pdf_to_dc_func_) << "Missing RenderPDFPageToDC";

    return render_pdf_to_dc_func_ != NULL;
  }

  bool RenderPDFPageToDC(const void* pdf_buffer,
                         int buffer_size,
                         int page_number,
                         HDC dc,
                         int dpi_x,
                         int dpi_y,
                         int bounds_origin_x,
                         int bounds_origin_y,
                         int bounds_width,
                         int bounds_height,
                         bool fit_to_bounds,
                         bool stretch_to_bounds,
                         bool keep_aspect_ratio,
                         bool center_in_bounds,
                         bool autorotate) {
    if (!render_pdf_to_dc_func_)
      return false;
    return render_pdf_to_dc_func_(pdf_buffer, buffer_size, page_number,
                                  dc, dpi_x, dpi_y, bounds_origin_x,
                                  bounds_origin_y, bounds_width, bounds_height,
                                  fit_to_bounds, stretch_to_bounds,
                                  keep_aspect_ratio, center_in_bounds,
                                  autorotate);
  }

 private:
  // Exported by PDF plugin.
  typedef bool (*RenderPDFPageToDCProc)(
      const void* pdf_buffer, int buffer_size, int page_number, HDC dc,
      int dpi_x, int dpi_y, int bounds_origin_x, int bounds_origin_y,
      int bounds_width, int bounds_height, bool fit_to_bounds,
      bool stretch_to_bounds, bool keep_aspect_ratio, bool center_in_bounds,
      bool autorotate);
  RenderPDFPageToDCProc render_pdf_to_dc_func_;

  DISALLOW_COPY_AND_ASSIGN(PdfFunctionsWin);
};

typedef PdfFunctionsWin PdfFunctions;
#else  // OS_WIN
typedef PdfFunctionsBase PdfFunctions;
#endif  // OS_WIN

base::LazyInstance<PdfFunctions> g_pdf_lib = LAZY_INSTANCE_INITIALIZER;

}  // namespace

PrintingHandler::PrintingHandler() {}

PrintingHandler::~PrintingHandler() {}

// static
void PrintingHandler::PreSandboxStartup() {
  g_pdf_lib.Get().Init();
}

bool PrintingHandler::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(PrintingHandler, message)
#if defined(WIN_PDF_METAFILE_FOR_PRINTING)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_RenderPDFPagesToMetafiles,
                        OnRenderPDFPagesToMetafile)
#endif
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_RenderPDFPagesToPWGRaster,
                        OnRenderPDFPagesToPWGRaster)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_GetPrinterCapsAndDefaults,
                        OnGetPrinterCapsAndDefaults)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_GetPrinterSemanticCapsAndDefaults,
                        OnGetPrinterSemanticCapsAndDefaults)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

#if defined(WIN_PDF_METAFILE_FOR_PRINTING)
void PrintingHandler::OnRenderPDFPagesToMetafile(
    IPC::PlatformFileForTransit pdf_transit,
    const base::FilePath& metafile_path,
    const printing::PdfRenderSettings& settings,
    const std::vector<printing::PageRange>& page_ranges_const) {
  bool succeeded = false;
  base::File pdf_file = IPC::PlatformFileForTransitToFile(pdf_transit);
  int highest_rendered_page_number = 0;
  double scale_factor = 1.0;
  std::vector<printing::PageRange> page_ranges = page_ranges_const;
  succeeded = RenderPDFToWinMetafile(pdf_file.Pass(),
                                     metafile_path,
                                     settings,
                                     &page_ranges,
                                     &highest_rendered_page_number,
                                     &scale_factor);
  if (succeeded) {
    // TODO(vitalybuka|scottmg): http://crbug.com/170859. These could
    // potentially be sent as each page is converted so that the spool could
    // start sooner.
    Send(new ChromeUtilityHostMsg_RenderPDFPagesToMetafiles_Succeeded(
        page_ranges, scale_factor));
  } else {
    Send(new ChromeUtilityHostMsg_RenderPDFPagesToMetafile_Failed());
  }
  ReleaseProcessIfNeeded();
}
#endif

void PrintingHandler::OnRenderPDFPagesToPWGRaster(
    IPC::PlatformFileForTransit pdf_transit,
    const printing::PdfRenderSettings& settings,
    const printing::PwgRasterSettings& bitmap_settings,
    IPC::PlatformFileForTransit bitmap_transit) {
  base::File pdf = IPC::PlatformFileForTransitToFile(pdf_transit);
  base::File bitmap = IPC::PlatformFileForTransitToFile(bitmap_transit);
  if (RenderPDFPagesToPWGRaster(pdf.Pass(), settings, bitmap_settings,
                                bitmap.Pass())) {
    Send(new ChromeUtilityHostMsg_RenderPDFPagesToPWGRaster_Succeeded());
  } else {
    Send(new ChromeUtilityHostMsg_RenderPDFPagesToPWGRaster_Failed());
  }
  ReleaseProcessIfNeeded();
}

#if defined(WIN_PDF_METAFILE_FOR_PRINTING)
bool PrintingHandler::RenderPDFToWinMetafile(
    base::File pdf_file,
    const base::FilePath& metafile_path,
    const printing::PdfRenderSettings& settings,
    std::vector<printing::PageRange>* page_ranges,
    int* highest_rendered_page_number,
    double* scale_factor) {
  DCHECK(page_ranges);
  *highest_rendered_page_number = -1;
  *scale_factor = 1.0;

  if (!g_pdf_lib.Get().IsValid())
    return false;

  // TODO(sanjeevr): Add a method to the PDF DLL that takes in a file handle
  // and a page range array. That way we don't need to read the entire PDF into
  // memory.
  int64 length = pdf_file.GetLength();
  if (length < 0)
    return false;

  std::vector<char> buffer;
  buffer.resize(length);
  if (length != pdf_file.Read(0, &buffer.front(), length))
    return false;

  int total_page_count = 0;
  if (!g_pdf_lib.Get().GetPDFDocInfo(&buffer.front(), buffer.size(),
                                     &total_page_count, NULL)) {
    return false;
  }

  // If no range supplied, do all pages.
  if (page_ranges->empty()) {
    printing::PageRange page_range_all;
    page_range_all.from = 0;
    page_range_all.to = total_page_count - 1;
    page_ranges->push_back(page_range_all);
  }

  bool ret = false;
  std::vector<printing::PageRange>::const_iterator iter;
  for (iter = page_ranges->begin(); iter != page_ranges->end(); ++iter) {
    for (int page_number = iter->from; page_number <= iter->to; ++page_number) {
      if (page_number >= total_page_count)
        break;

      printing::Emf metafile;
      metafile.InitToFile(metafile_path.InsertBeforeExtensionASCII(
          base::StringPrintf(".%d", page_number)));

      // We need to scale down DC to fit an entire page into DC available area.
      // Current metafile is based on screen DC and have current screen size.
      // Writing outside of those boundaries will result in the cut-off output.
      // On metafiles (this is the case here), scaling down will still record
      // original coordinates and we'll be able to print in full resolution.
      // Before playback we'll need to counter the scaling up that will happen
      // in the service (print_system_win.cc).
      *scale_factor = gfx::CalculatePageScale(metafile.context(),
                                              settings.area().right(),
                                              settings.area().bottom());
      gfx::ScaleDC(metafile.context(), *scale_factor);

      // The underlying metafile is of type Emf and ignores the arguments passed
      // to StartPage.
      metafile.StartPage(gfx::Size(), gfx::Rect(), 1);
      if (g_pdf_lib.Get().RenderPDFPageToDC(
              &buffer.front(), buffer.size(), page_number, metafile.context(),
              settings.dpi(), settings.dpi(), settings.area().x(),
              settings.area().y(), settings.area().width(),
              settings.area().height(), true, false, true, true,
              settings.autorotate())) {
        if (*highest_rendered_page_number < page_number)
          *highest_rendered_page_number = page_number;
        ret = true;
      }
      metafile.FinishPage();
      metafile.FinishDocument();
    }
  }
  return ret;
}
#endif  // defined(WIN_PDF_METAFILE_FOR_PRINTING)

bool PrintingHandler::RenderPDFPagesToPWGRaster(
    base::File pdf_file,
    const printing::PdfRenderSettings& settings,
    const printing::PwgRasterSettings& bitmap_settings,
    base::File bitmap_file) {
  bool autoupdate = true;
  if (!g_pdf_lib.Get().IsValid())
    return false;

  base::File::Info info;
  if (!pdf_file.GetInfo(&info) || info.size <= 0)
    return false;

  std::string data(info.size, 0);
  int data_size = pdf_file.Read(0, &data[0], data.size());
  if (data_size != static_cast<int>(data.size()))
    return false;

  int total_page_count = 0;
  if (!g_pdf_lib.Get().GetPDFDocInfo(data.data(), data.size(),
                                     &total_page_count, NULL)) {
    return false;
  }

  cloud_print::PwgEncoder encoder;
  std::string pwg_header;
  encoder.EncodeDocumentHeader(&pwg_header);
  int bytes_written = bitmap_file.WriteAtCurrentPos(pwg_header.data(),
                                                    pwg_header.size());
  if (bytes_written != static_cast<int>(pwg_header.size()))
    return false;

  cloud_print::BitmapImage image(settings.area().size(),
                                 cloud_print::BitmapImage::BGRA);
  for (int i = 0; i < total_page_count; ++i) {
    int page_number = i;

    if (bitmap_settings.reverse_page_order) {
      page_number = total_page_count - 1 - page_number;
    }

    if (!g_pdf_lib.Get().RenderPDFPageToBitmap(data.data(),
                                               data.size(),
                                               page_number,
                                               image.pixel_data(),
                                               image.size().width(),
                                               image.size().height(),
                                               settings.dpi(),
                                               settings.dpi(),
                                               autoupdate)) {
      return false;
    }

    cloud_print::PwgHeaderInfo header_info;
    header_info.dpi = settings.dpi();
    header_info.total_pages = total_page_count;

    // Transform odd pages.
    if (page_number % 2) {
      switch (bitmap_settings.odd_page_transform) {
        case printing::TRANSFORM_NORMAL:
          break;
        case printing::TRANSFORM_ROTATE_180:
          header_info.flipx = true;
          header_info.flipy = true;
          break;
        case printing::TRANSFORM_FLIP_HORIZONTAL:
          header_info.flipx = true;
          break;
        case printing::TRANSFORM_FLIP_VERTICAL:
          header_info.flipy = true;
          break;
      }
    }

    if (bitmap_settings.rotate_all_pages) {
      header_info.flipx = !header_info.flipx;
      header_info.flipy = !header_info.flipy;
    }

    std::string pwg_page;
    if (!encoder.EncodePage(image, header_info, &pwg_page))
      return false;
    bytes_written = bitmap_file.WriteAtCurrentPos(pwg_page.data(),
                                                  pwg_page.size());
    if (bytes_written != static_cast<int>(pwg_page.size()))
      return false;
  }
  return true;
}

void PrintingHandler::OnGetPrinterCapsAndDefaults(
    const std::string& printer_name) {
  scoped_refptr<printing::PrintBackend> print_backend =
      printing::PrintBackend::CreateInstance(NULL);
  printing::PrinterCapsAndDefaults printer_info;

  crash_keys::ScopedPrinterInfo crash_key(
      print_backend->GetPrinterDriverInfo(printer_name));

  if (print_backend->GetPrinterCapsAndDefaults(printer_name, &printer_info)) {
    Send(new ChromeUtilityHostMsg_GetPrinterCapsAndDefaults_Succeeded(
        printer_name, printer_info));
  } else  {
    Send(new ChromeUtilityHostMsg_GetPrinterCapsAndDefaults_Failed(
        printer_name));
  }
  ReleaseProcessIfNeeded();
}

void PrintingHandler::OnGetPrinterSemanticCapsAndDefaults(
    const std::string& printer_name) {
  scoped_refptr<printing::PrintBackend> print_backend =
      printing::PrintBackend::CreateInstance(NULL);
  printing::PrinterSemanticCapsAndDefaults printer_info;

  crash_keys::ScopedPrinterInfo crash_key(
      print_backend->GetPrinterDriverInfo(printer_name));

  if (print_backend->GetPrinterSemanticCapsAndDefaults(printer_name,
                                                       &printer_info)) {
    Send(new ChromeUtilityHostMsg_GetPrinterSemanticCapsAndDefaults_Succeeded(
        printer_name, printer_info));
  } else {
    Send(new ChromeUtilityHostMsg_GetPrinterSemanticCapsAndDefaults_Failed(
        printer_name));
  }
  ReleaseProcessIfNeeded();
}
