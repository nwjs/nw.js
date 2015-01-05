// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/printing/print_dialog_gtk.h"

#include <fcntl.h>
#include <gtk/gtkunixprint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include "base/bind.h"
#include "base/file_util.h"
#include "base/files/file_util_proxy.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/message_loop/message_loop_proxy.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "printing/metafile.h"
#include "printing/print_job_constants.h"
#include "printing/print_settings.h"
#include "printing/print_settings_initializer_gtk.h"

using content::BrowserThread;
using printing::PageRanges;
using printing::PrintSettings;

namespace {

// CUPS Duplex attribute and values.
const char kCUPSDuplex[] = "cups-Duplex";
const char kDuplexNone[] = "None";
const char kDuplexTumble[] = "DuplexTumble";
const char kDuplexNoTumble[] = "DuplexNoTumble";

class StickyPrintSettingGtk {
 public:
  StickyPrintSettingGtk() : last_used_settings_(gtk_print_settings_new()) {
  }
  ~StickyPrintSettingGtk() {
    NOTREACHED();  // Intended to be used with a Leaky LazyInstance.
  }

  GtkPrintSettings* settings() {
    return last_used_settings_;
  }

  void SetLastUsedSettings(GtkPrintSettings* settings) {
    DCHECK(last_used_settings_);
    g_object_unref(last_used_settings_);
    last_used_settings_ = gtk_print_settings_copy(settings);
  }

 private:
  GtkPrintSettings* last_used_settings_;

  DISALLOW_COPY_AND_ASSIGN(StickyPrintSettingGtk);
};

base::LazyInstance<StickyPrintSettingGtk>::Leaky g_last_used_settings =
    LAZY_INSTANCE_INITIALIZER;

// Helper class to track GTK printers.
class GtkPrinterList {
 public:
  GtkPrinterList() : default_printer_(NULL) {
    gtk_enumerate_printers(SetPrinter, this, NULL, TRUE);
  }

  ~GtkPrinterList() {
    for (std::vector<GtkPrinter*>::iterator it = printers_.begin();
         it < printers_.end(); ++it) {
      g_object_unref(*it);
    }
  }

  // Can return NULL if there's no default printer. E.g. Printer on a laptop
  // is "home_printer", but the laptop is at work.
  GtkPrinter* default_printer() {
    return default_printer_;
  }

  // Can return NULL if the printer cannot be found due to:
  // - Printer list out of sync with printer dialog UI.
  // - Querying for non-existant printers like 'Print to PDF'.
  GtkPrinter* GetPrinterWithName(const std::string& name) {
    if (name.empty())
      return NULL;

    for (std::vector<GtkPrinter*>::iterator it = printers_.begin();
         it < printers_.end(); ++it) {
      if (gtk_printer_get_name(*it) == name) {
        return *it;
      }
    }

    return NULL;
  }

 private:
  // Callback function used by gtk_enumerate_printers() to get all printer.
  static gboolean SetPrinter(GtkPrinter* printer, gpointer data) {
    GtkPrinterList* printer_list = reinterpret_cast<GtkPrinterList*>(data);
    if (gtk_printer_is_default(printer))
      printer_list->default_printer_ = printer;

    g_object_ref(printer);
    printer_list->printers_.push_back(printer);

    return FALSE;
  }

  std::vector<GtkPrinter*> printers_;
  GtkPrinter* default_printer_;
};

}  // namespace

// static
printing::PrintDialogGtkInterface* PrintDialogGtk::CreatePrintDialog(
    PrintingContextLinux* context) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  return new PrintDialogGtk(context);
}

PrintDialogGtk::PrintDialogGtk(PrintingContextLinux* context)
    : context_(context),
      dialog_(NULL),
      gtk_settings_(NULL),
      page_setup_(NULL),
      printer_(NULL) {
}

PrintDialogGtk::~PrintDialogGtk() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (dialog_) {
    gtk_widget_destroy(dialog_);
    dialog_ = NULL;
  }
  if (gtk_settings_) {
    g_object_unref(gtk_settings_);
    gtk_settings_ = NULL;
  }
  if (page_setup_) {
    g_object_unref(page_setup_);
    page_setup_ = NULL;
  }
  if (printer_) {
    g_object_unref(printer_);
    printer_ = NULL;
  }
}

void PrintDialogGtk::UseDefaultSettings() {
  DCHECK(!page_setup_);
  DCHECK(!printer_);

  // |gtk_settings_| is a new object.
  gtk_settings_ = gtk_print_settings_new();
  page_setup_ = gtk_page_setup_new();

  // No page range to initialize for default settings.
  PrintSettings settings;
  InitPrintSettings(&settings);
}

bool PrintDialogGtk::UpdateSettings(printing::PrintSettings* settings) {
  if (!gtk_settings_) {
    gtk_settings_ =
        gtk_print_settings_copy(g_last_used_settings.Get().settings());
  }

  scoped_ptr<GtkPrinterList> printer_list(new GtkPrinterList);
  printer_ = printer_list->GetPrinterWithName(
      UTF16ToUTF8(settings->device_name()));
  if (printer_) {
    g_object_ref(printer_);
    gtk_print_settings_set_printer(gtk_settings_,
                                   gtk_printer_get_name(printer_));
    if (!page_setup_) {
      page_setup_ = gtk_printer_get_default_page_size(printer_);
    }
  }

  gtk_print_settings_set_n_copies(gtk_settings_, settings->copies());
  gtk_print_settings_set_collate(gtk_settings_, settings->collate());

#if defined(USE_CUPS)
  std::string color_value;
  std::string color_setting_name;
  printing::GetColorModelForMode(settings->color(), &color_setting_name,
                                 &color_value);
  gtk_print_settings_set(gtk_settings_, color_setting_name.c_str(),
                         color_value.c_str());

  if (settings->duplex_mode() != printing::UNKNOWN_DUPLEX_MODE) {
    const char* cups_duplex_mode = NULL;
    switch (settings->duplex_mode()) {
      case printing::LONG_EDGE:
        cups_duplex_mode = kDuplexNoTumble;
        break;
      case printing::SHORT_EDGE:
        cups_duplex_mode = kDuplexTumble;
        break;
      case printing::SIMPLEX:
        cups_duplex_mode = kDuplexNone;
        break;
      default:  // UNKNOWN_DUPLEX_MODE
        NOTREACHED();
        break;
    }
    gtk_print_settings_set(gtk_settings_, kCUPSDuplex, cups_duplex_mode);
  }
#endif
  if (!page_setup_)
    page_setup_ = gtk_page_setup_new();

  gtk_print_settings_set_orientation(
      gtk_settings_,
      settings->landscape() ? GTK_PAGE_ORIENTATION_LANDSCAPE :
                              GTK_PAGE_ORIENTATION_PORTRAIT);

  InitPrintSettings(settings);
  return true;
}

void PrintDialogGtk::ShowDialog(
    gfx::NativeView parent_view,
    bool has_selection,
    const PrintingContextLinux::PrintSettingsCallback& callback) {
  callback_ = callback;

  GtkWindow* parent = GTK_WINDOW(gtk_widget_get_toplevel(parent_view));
  // TODO(estade): We need a window title here.
  dialog_ = gtk_print_unix_dialog_new(NULL, parent);
  g_signal_connect(dialog_, "delete-event",
                   G_CALLBACK(gtk_widget_hide_on_delete), NULL);


  // Set modal so user cannot focus the same tab and press print again.
  gtk_window_set_modal(GTK_WINDOW(dialog_), TRUE);

  // Since we only generate PDF, only show printers that support PDF.
  // TODO(thestig) Add more capabilities to support?
  GtkPrintCapabilities cap = static_cast<GtkPrintCapabilities>(
      GTK_PRINT_CAPABILITY_GENERATE_PDF |
      GTK_PRINT_CAPABILITY_PAGE_SET |
      GTK_PRINT_CAPABILITY_COPIES |
      GTK_PRINT_CAPABILITY_COLLATE |
      GTK_PRINT_CAPABILITY_REVERSE);
  gtk_print_unix_dialog_set_manual_capabilities(GTK_PRINT_UNIX_DIALOG(dialog_),
                                                cap);
  gtk_print_unix_dialog_set_embed_page_setup(GTK_PRINT_UNIX_DIALOG(dialog_),
                                             TRUE);
  gtk_print_unix_dialog_set_support_selection(GTK_PRINT_UNIX_DIALOG(dialog_),
                                              TRUE);
  gtk_print_unix_dialog_set_has_selection(GTK_PRINT_UNIX_DIALOG(dialog_),
                                          has_selection);
  g_signal_connect(dialog_, "response", G_CALLBACK(OnResponseThunk), this);
  gtk_widget_show(dialog_);
}

void PrintDialogGtk::PrintDocument(const printing::Metafile* metafile,
                                   const base::string16& document_name) {
  // This runs on the print worker thread, does not block the UI thread.
  DCHECK(!BrowserThread::CurrentlyOn(BrowserThread::UI));

  // The document printing tasks can outlive the PrintingContext that created
  // this dialog.
  AddRef();

  bool error = false;
  if (!base::CreateTemporaryFile(&path_to_pdf_)) {
    LOG(ERROR) << "Creating temporary file failed";
    error = true;
  }

  if (!error && !metafile->SaveTo(path_to_pdf_)) {
    LOG(ERROR) << "Saving metafile failed";
    base::DeleteFile(path_to_pdf_, false);
    error = true;
  }

  if (error) {
    // Matches AddRef() above.
    Release();
  } else {
    // No errors, continue printing.
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&PrintDialogGtk::SendDocumentToPrinter, this,
                   document_name));
  }
}

void PrintDialogGtk::AddRefToDialog() {
  AddRef();
}

void PrintDialogGtk::ReleaseDialog() {
  Release();
}

void PrintDialogGtk::OnResponse(GtkWidget* dialog, int response_id) {
  gtk_widget_hide(dialog_);

  switch (response_id) {
    case GTK_RESPONSE_OK: {
      if (gtk_settings_)
        g_object_unref(gtk_settings_);
      gtk_settings_ = gtk_print_unix_dialog_get_settings(
          GTK_PRINT_UNIX_DIALOG(dialog_));

      if (printer_)
        g_object_unref(printer_);
      printer_ = gtk_print_unix_dialog_get_selected_printer(
          GTK_PRINT_UNIX_DIALOG(dialog_));
      g_object_ref(printer_);

      if (page_setup_)
        g_object_unref(page_setup_);
      page_setup_ = gtk_print_unix_dialog_get_page_setup(
          GTK_PRINT_UNIX_DIALOG(dialog_));
      g_object_ref(page_setup_);

      // Handle page ranges.
      PageRanges ranges_vector;
      gint num_ranges;
      bool print_selection_only = false;
      switch (gtk_print_settings_get_print_pages(gtk_settings_)) {
        case GTK_PRINT_PAGES_RANGES: {
          GtkPageRange* gtk_range =
              gtk_print_settings_get_page_ranges(gtk_settings_, &num_ranges);
          if (gtk_range) {
            for (int i = 0; i < num_ranges; ++i) {
              printing::PageRange range;
              range.from = gtk_range[i].start;
              range.to = gtk_range[i].end;
              ranges_vector.push_back(range);
            }
            g_free(gtk_range);
          }
          break;
        }
        case GTK_PRINT_PAGES_SELECTION:
          print_selection_only = true;
          break;
        case GTK_PRINT_PAGES_ALL:
          // Leave |ranges_vector| empty to indicate print all pages.
          break;
        case GTK_PRINT_PAGES_CURRENT:
        default:
          NOTREACHED();
          break;
      }

      PrintSettings settings;
      settings.set_ranges(ranges_vector);
      settings.set_selection_only(print_selection_only);
      printing::PrintSettingsInitializerGtk::InitPrintSettings(
          gtk_settings_, page_setup_, &settings);
      context_->InitWithSettings(settings);
      callback_.Run(PrintingContextLinux::OK);
      callback_.Reset();
      return;
    }
    case GTK_RESPONSE_DELETE_EVENT:  // Fall through.
    case GTK_RESPONSE_CANCEL: {
      callback_.Run(PrintingContextLinux::CANCEL);
      callback_.Reset();
      return;
    }
    case GTK_RESPONSE_APPLY:
    default: {
      NOTREACHED();
    }
  }
}

void PrintDialogGtk::SendDocumentToPrinter(const base::string16& document_name) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // If |printer_| is NULL then somehow the GTK printer list changed out under
  // us. In which case, just bail out.
  if (!printer_) {
    // Matches AddRef() in PrintDocument();
    Release();
    return;
  }

  GtkPrintJob* print_job = gtk_print_job_new(
      UTF16ToUTF8(document_name).c_str(),
      printer_,
      gtk_settings_,
      page_setup_);
  gtk_print_job_set_source_file(print_job, path_to_pdf_.value().c_str(), NULL);
  gtk_print_job_send(print_job, OnJobCompletedThunk, this, NULL);
}

// static
void PrintDialogGtk::OnJobCompletedThunk(GtkPrintJob* print_job,
                                         gpointer user_data,
                                         GError* error) {
  static_cast<PrintDialogGtk*>(user_data)->OnJobCompleted(print_job, error);
}

void PrintDialogGtk::OnJobCompleted(GtkPrintJob* print_job, GError* error) {
  if (error)
    LOG(ERROR) << "Printing failed: " << error->message;
  if (print_job)
    g_object_unref(print_job);
  base::FileUtilProxy::DeleteFile(
      BrowserThread::GetMessageLoopProxyForThread(BrowserThread::FILE).get(),
      path_to_pdf_, false, base::FileUtilProxy::StatusCallback());
  // Printing finished. Matches AddRef() in PrintDocument();
  Release();
}

void PrintDialogGtk::InitPrintSettings(PrintSettings* settings) {
  printing::PrintSettingsInitializerGtk::InitPrintSettings(
      gtk_settings_, page_setup_, settings);
  context_->InitWithSettings(*settings);
}
