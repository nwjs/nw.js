// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/print_messages.h"

#include "base/basictypes.h"
#include "base/strings/string16.h"
#include "ui/gfx/size.h"

PrintMsg_Print_Params::PrintMsg_Print_Params()
  : page_size(),
    content_size(),
    printable_area(),
    margin_top(0),
    margin_left(0),
    dpi(0),
    min_shrink(0),
    max_shrink(0),
    desired_dpi(0),
    document_cookie(0),
    selection_only(false),
    supports_alpha_blend(false),
    preview_ui_id(-1),
    preview_request_id(0),
    is_first_request(false),
    print_scaling_option(blink::WebPrintScalingOptionSourceSize),
    print_to_pdf(false),
    display_header_footer(false),
    title(),
    url(),
    should_print_backgrounds(false) {
}

PrintMsg_Print_Params::~PrintMsg_Print_Params() {}

void PrintMsg_Print_Params::Reset() {
  page_size = gfx::Size();
  content_size = gfx::Size();
  printable_area = gfx::Rect();
  margin_top = 0;
  margin_left = 0;
  dpi = 0;
  min_shrink = 0;
  max_shrink = 0;
  desired_dpi = 0;
  document_cookie = 0;
  selection_only = false;
  supports_alpha_blend = false;
  preview_ui_id = -1;
  preview_request_id = 0;
  is_first_request = false;
  print_scaling_option = blink::WebPrintScalingOptionSourceSize;
  print_to_pdf = false;
  display_header_footer = false;
  title = base::string16();
  url = base::string16();
  should_print_backgrounds = false;
}

PrintMsg_PrintPages_Params::PrintMsg_PrintPages_Params()
  : pages() {
}

PrintMsg_PrintPages_Params::~PrintMsg_PrintPages_Params() {}

void PrintMsg_PrintPages_Params::Reset() {
  params.Reset();
  pages = std::vector<int>();
}

PrintHostMsg_RequestPrintPreview_Params::
    PrintHostMsg_RequestPrintPreview_Params()
    : is_modifiable(false),
      webnode_only(false),
      has_selection(false),
      selection_only(false) {
}

PrintHostMsg_RequestPrintPreview_Params::
    ~PrintHostMsg_RequestPrintPreview_Params() {}

PrintHostMsg_SetOptionsFromDocument_Params::
    PrintHostMsg_SetOptionsFromDocument_Params()
    : is_scaling_disabled(false),
      copies(0),
      duplex(printing::UNKNOWN_DUPLEX_MODE) {
}

PrintHostMsg_SetOptionsFromDocument_Params::
    ~PrintHostMsg_SetOptionsFromDocument_Params() {
}
