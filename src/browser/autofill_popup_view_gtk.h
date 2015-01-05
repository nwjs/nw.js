// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_GTK_AUTOFILL_AUTOFILL_POPUP_VIEW_GTK_H_
#define CHROME_BROWSER_UI_GTK_AUTOFILL_AUTOFILL_POPUP_VIEW_GTK_H_

#include <pango/pango.h>
#include <vector>

#include "base/compiler_specific.h"
#include "base/strings/string16.h"
#include "chrome/browser/ui/autofill/autofill_popup_view.h"
#include "ui/base/glib/glib_integers.h"
#include "ui/base/gtk/gtk_signal.h"
#include "ui/base/gtk/gtk_signal_registrar.h"

class Profile;

namespace content {
class RenderViewHost;
}

namespace gfx {
class FontList;
class Rect;
}

typedef struct _GdkEventButton GdkEventButton;
typedef struct _GdkEventConfigure GdkEventConfigure;
typedef struct _GdkEventCrossing GdkEventCrossing;
typedef struct _GdkEventExpose GdkEventExpose;
typedef struct _GdkEventKey GdkEventKey;
typedef struct _GdkEventMotion GdkEventMotion;
typedef struct _GdkColor GdkColor;
typedef struct _GtkWidget GtkWidget;

namespace autofill {

class AutofillPopupController;

// Gtk implementation for AutofillPopupView interface.
class AutofillPopupViewGtk : public AutofillPopupView {
 public:
  explicit AutofillPopupViewGtk(AutofillPopupController* controller);

 private:
  virtual ~AutofillPopupViewGtk();

  // AutofillPopupView implementation.
  virtual void Hide() OVERRIDE;
  virtual void Show() OVERRIDE;
  virtual void InvalidateRow(size_t row) OVERRIDE;
  virtual void UpdateBoundsAndRedrawPopup() OVERRIDE;

  CHROMEGTK_CALLBACK_1(AutofillPopupViewGtk, gboolean, HandleConfigure,
                       GdkEventConfigure*);
  CHROMEGTK_CALLBACK_1(AutofillPopupViewGtk, gboolean, HandleButtonRelease,
                       GdkEventButton*);
  CHROMEGTK_CALLBACK_1(AutofillPopupViewGtk, gboolean, HandleExpose,
                       GdkEventExpose*);
  CHROMEGTK_CALLBACK_1(AutofillPopupViewGtk, gboolean, HandleLeave,
                       GdkEventCrossing*)
  CHROMEGTK_CALLBACK_1(AutofillPopupViewGtk, gboolean, HandleMotion,
                       GdkEventMotion*);

  // Set up the pango layout to display the autofill results.
  void SetUpLayout();

  // Set up the pango layout to print the given text and have it's width match
  // the text's (this gives us better control when placing the text box).
  void SetLayoutText(const base::string16& text,
                     const gfx::FontList& font_list,
                     const GdkColor text_color);

  // Draw the separator as the given |separator_rect|.
  void DrawSeparator(cairo_t* cairo_context, const gfx::Rect& separator_rect);

  // Draw the given autofill entry in |entry_rect|.
  void DrawAutofillEntry(cairo_t* cairo_context,
                         size_t index,
                         const gfx::Rect& entry_rect);

  // Set the initial bounds of the popup to show, including the placement
  // of it.
  void SetInitialBounds();

  AutofillPopupController* controller_;  // Weak reference.

  ui::GtkSignalRegistrar signals_;

  GtkWidget* window_;  // Strong reference.
  PangoLayout* layout_;  // Strong reference.

  DISALLOW_COPY_AND_ASSIGN(AutofillPopupViewGtk);
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_GTK_AUTOFILL_AUTOFILL_POPUP_VIEW_GTK_H_
