// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/interstitials/security_interstitial_page.h"

#include "base/i18n/rtl.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "content/public/browser/interstitial_page.h"
#include "content/public/browser/web_contents.h"
#include "grit/nw_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/webui/jstemplate_builder.h"
#include "ui/base/webui/web_ui_util.h"

SecurityInterstitialPage::SecurityInterstitialPage(
    content::WebContents* web_contents,
    const GURL& request_url)
    : web_contents_(web_contents),
      request_url_(request_url),
      interstitial_page_(NULL),
      create_view_(true) {
  // Creating interstitial_page_ without showing it leaks memory, so don't
  // create it here.
}

SecurityInterstitialPage::~SecurityInterstitialPage() {
}

content::InterstitialPage* SecurityInterstitialPage::interstitial_page() const {
  return interstitial_page_;
}

content::WebContents* SecurityInterstitialPage::web_contents() const {
  return web_contents_;
}

GURL SecurityInterstitialPage::request_url() const {
  return request_url_;
}

void SecurityInterstitialPage::DontCreateViewForTesting() {
  create_view_ = false;
}

void SecurityInterstitialPage::Show() {
  DCHECK(!interstitial_page_);
  interstitial_page_ = content::InterstitialPage::Create(
      web_contents_, ShouldCreateNewNavigation(), request_url_, this);
  if (!create_view_)
    interstitial_page_->DontCreateViewForTesting();
  interstitial_page_->Show();
}

base::string16 SecurityInterstitialPage::GetFormattedHostName() const {
  base::string16 host(base::UTF8ToUTF16(request_url_.host()));
  if (base::i18n::IsRTL())
    base::i18n::WrapStringWithLTRFormatting(&host);
  return host;
}

std::string SecurityInterstitialPage::GetHTMLContents() {
  base::DictionaryValue load_time_data;
  PopulateInterstitialStrings(&load_time_data);
  webui::SetFontAndTextDirection(&load_time_data);
  base::StringPiece html(
      ResourceBundle::GetSharedInstance().GetRawDataResource(
          IDR_SECURITY_INTERSTITIAL_HTML));
  return webui::GetI18nTemplateHtml(html, &load_time_data);
}
