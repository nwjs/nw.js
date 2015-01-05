// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "login_view.h"

#include "base/strings/utf_string_conversions.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/layout/layout_constants.h"

static const int kMessageWidth = 320;
static const int kTextfieldStackHorizontalSpacing = 30;

using views::GridLayout;

///////////////////////////////////////////////////////////////////////////////
// LoginView, public:

LoginView::LoginView(const base::string16& explanation)
    : username_field_(new views::Textfield()),
      password_field_(new views::Textfield()),
      username_label_(new views::Label(base::ASCIIToUTF16("Username"))),
      password_label_(new views::Label(base::ASCIIToUTF16("Password"))),
      message_label_(new views::Label(explanation)) {
  password_field_->SetTextInputType(ui::TEXT_INPUT_TYPE_PASSWORD);
  message_label_->SetMultiLine(true);
  message_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  message_label_->SetAllowCharacterBreak(true);

  // Initialize the Grid Layout Manager used for this dialog box.
  GridLayout* layout = GridLayout::CreatePanel(this);
  SetLayoutManager(layout);

  // Add the column set for the information message at the top of the dialog
  // box.
  const int single_column_view_set_id = 0;
  views::ColumnSet* column_set =
      layout->AddColumnSet(single_column_view_set_id);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::FIXED, kMessageWidth, 0);

  // Add the column set for the user name and password fields and labels.
  const int labels_column_set_id = 1;
  column_set = layout->AddColumnSet(labels_column_set_id);
  column_set->AddPaddingColumn(0, kTextfieldStackHorizontalSpacing);
  column_set->AddColumn(GridLayout::LEADING, GridLayout::CENTER, 0,
                        GridLayout::USE_PREF, 0, 0);
  column_set->AddPaddingColumn(0, views::kRelatedControlHorizontalSpacing);
  column_set->AddColumn(GridLayout::FILL, GridLayout::CENTER, 1,
                        GridLayout::USE_PREF, 0, 0);
  column_set->AddPaddingColumn(0, kTextfieldStackHorizontalSpacing);

  layout->StartRow(0, single_column_view_set_id);
  layout->AddView(message_label_);

  layout->AddPaddingRow(0, views::kUnrelatedControlLargeVerticalSpacing);

  layout->StartRow(0, labels_column_set_id);
  layout->AddView(username_label_);
  layout->AddView(username_field_);

  layout->AddPaddingRow(0, views::kRelatedControlVerticalSpacing);

  layout->StartRow(0, labels_column_set_id);
  layout->AddView(password_label_);
  layout->AddView(password_field_);

  layout->AddPaddingRow(0, views::kUnrelatedControlVerticalSpacing);

}

LoginView::~LoginView() {
}

const base::string16& LoginView::GetUsername() const {
  return username_field_->text();
}

const base::string16& LoginView::GetPassword() const {
  return password_field_->text();
}

views::View* LoginView::GetInitiallyFocusedView() {
  return username_field_;
}

