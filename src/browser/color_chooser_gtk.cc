// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtk/gtk.h>

#include "content/public/browser/color_chooser.h"
#include "content/public/browser/web_contents.h"
#include "grit/nw_resources.h"
#include "ui/base/gtk/gtk_signal.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/gfx/skia_utils_gtk.h"

class ColorChooserGtk : public content::ColorChooser {
 public:
  static ColorChooserGtk* Open(content::WebContents* web_contents,
                               SkColor initial_color);

  ColorChooserGtk(content::WebContents* web_contents, SkColor initial_color);
  virtual ~ColorChooserGtk();

  virtual void End() OVERRIDE;
  virtual void SetSelectedColor(SkColor color) OVERRIDE;

 private:
  static ColorChooserGtk* current_color_chooser_;

  CHROMEGTK_CALLBACK_0(ColorChooserGtk, void, OnColorChooserOk);
  CHROMEGTK_CALLBACK_0(ColorChooserGtk, void, OnColorChooserCancel);
  CHROMEGTK_CALLBACK_0(ColorChooserGtk, void, OnColorChooserDestroy);

  // The web contents invoking the color chooser.  No ownership because it will
  // outlive this class.
  content::WebContents* web_contents_;
  GtkWidget* color_selection_dialog_;
};

ColorChooserGtk* ColorChooserGtk::current_color_chooser_ = NULL;

ColorChooserGtk* ColorChooserGtk::Open(content::WebContents* web_contents,
                                       SkColor initial_color) {
  if (current_color_chooser_)
    current_color_chooser_->End();
  DCHECK(!current_color_chooser_);
  current_color_chooser_ = new ColorChooserGtk(web_contents, initial_color);
  return current_color_chooser_;
}

ColorChooserGtk::ColorChooserGtk(content::WebContents* web_contents,
                                 SkColor initial_color)
    : web_contents_(web_contents) {
  color_selection_dialog_ = gtk_color_selection_dialog_new(
      l10n_util::GetStringUTF8(IDS_SELECT_COLOR_DIALOG_TITLE).c_str());
  GtkWidget* cancel_button;
  GtkColorSelection* color_selection;
  GtkWidget* ok_button;
  g_object_get(color_selection_dialog_,
               "cancel-button", &cancel_button,
               "color-selection", &color_selection,
               "ok-button", &ok_button,
               NULL);
  gtk_color_selection_set_has_opacity_control(color_selection, FALSE);
  g_signal_connect(ok_button, "clicked",
                   G_CALLBACK(OnColorChooserOkThunk), this);
  g_signal_connect(cancel_button, "clicked",
                   G_CALLBACK(OnColorChooserCancelThunk), this);
  g_signal_connect(color_selection_dialog_, "destroy",
                   G_CALLBACK(OnColorChooserDestroyThunk), this);
  GdkColor gdk_color = gfx::SkColorToGdkColor(initial_color);
  gtk_color_selection_set_previous_color(color_selection, &gdk_color);
  gtk_color_selection_set_current_color(color_selection, &gdk_color);
  gtk_window_present(GTK_WINDOW(color_selection_dialog_));
  g_object_unref(cancel_button);
  g_object_unref(color_selection);
  g_object_unref(ok_button);
}

ColorChooserGtk::~ColorChooserGtk() {
  // Always call End() before destroying.
  DCHECK(!color_selection_dialog_);
}

void ColorChooserGtk::OnColorChooserOk(GtkWidget* widget) {
  GdkColor color;
  GtkColorSelection* color_selection;
  g_object_get(color_selection_dialog_,
               "color-selection", &color_selection, NULL);
  gtk_color_selection_get_current_color(color_selection, &color);
  if (web_contents_)
    web_contents_->DidChooseColorInColorChooser(gfx::GdkColorToSkColor(color));
  g_object_unref(color_selection);
  gtk_widget_destroy(color_selection_dialog_);
}

void ColorChooserGtk::OnColorChooserCancel(GtkWidget* widget) {
  gtk_widget_destroy(color_selection_dialog_);
}

void ColorChooserGtk::OnColorChooserDestroy(GtkWidget* widget) {
  color_selection_dialog_ = NULL;
  DCHECK(current_color_chooser_ == this);
  current_color_chooser_ = NULL;
  if (web_contents_)
    web_contents_->DidEndColorChooser();
}

void ColorChooserGtk::End() {
  if (!color_selection_dialog_)
    return;

  gtk_widget_destroy(color_selection_dialog_);
}

void ColorChooserGtk::SetSelectedColor(SkColor color) {
  if (!color_selection_dialog_)
    return;

  GdkColor gdk_color = gfx::SkColorToGdkColor(color);
  GtkColorSelection* color_selection;
  g_object_get(color_selection_dialog_,
               "color-selection", &color_selection, NULL);
  gtk_color_selection_set_previous_color(color_selection, &gdk_color);
  gtk_color_selection_set_current_color(color_selection, &gdk_color);
  g_object_unref(color_selection);
}

namespace nw {

content::ColorChooser* ShowColorChooser(content::WebContents* web_contents,
                                        SkColor initial_color) {
  return ColorChooserGtk::Open(web_contents, initial_color);
}

}  // namespace chrome
