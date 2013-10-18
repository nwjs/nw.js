// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/renderer/autofill_agent.h"

#include "base/bind.h"
#include "base/message_loop.h"
#include "base/string_util.h"
#include "base/strings/string_split.h"
#include "content/public/renderer/render_view.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebInputEvent.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebNode.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebNodeCollection.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebOptionElement.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include "ui/base/keycodes/keyboard_codes.h"

using WebKit::WebAutofillClient;
using WebKit::WebFormElement;
using WebKit::WebFrame;
using WebKit::WebInputElement;
using WebKit::WebKeyboardEvent;
using WebKit::WebNode;
using WebKit::WebNodeCollection;
using WebKit::WebOptionElement;
using WebKit::WebString;

namespace nw {

namespace {

// The size above which we stop triggering autofill for an input text field
// (so to avoid sending long strings through IPC).
const size_t kMaximumTextSizeForAutofill = 1000;

void AppendDataListSuggestions(const WebKit::WebInputElement& element,
                               std::vector<string16>* values,
                               std::vector<string16>* labels,
                               std::vector<string16>* icons,
                               std::vector<int>* item_ids) {
  WebNodeCollection options = element.dataListOptions();
  if (options.isNull())
    return;

  string16 prefix = element.editingValue();
  if (element.isMultiple() &&
      element.formControlType() == WebString::fromUTF8("email")) {
    std::vector<string16> parts;
    base::SplitStringDontTrim(prefix, ',', &parts);
    if (parts.size() > 0)
      TrimWhitespace(parts[parts.size() - 1], TRIM_LEADING, &prefix);
  }
  for (WebOptionElement option = options.firstItem().to<WebOptionElement>();
       !option.isNull(); option = options.nextItem().to<WebOptionElement>()) {
    if (!StartsWith(option.value(), prefix, false) ||
        option.value() == prefix ||
        !element.isValidValue(option.value()))
      continue;

    values->push_back(option.value());
    if (option.value() != option.label())
      labels->push_back(option.label());
    else
      labels->push_back(string16());
    icons->push_back(string16());
    item_ids->push_back(WebAutofillClient::MenuItemIDDataListEntry);
  }
}

}  // namespace

AutofillAgent::AutofillAgent(content::RenderView* render_view)
    : content::RenderViewObserver(render_view),
      ALLOW_THIS_IN_INITIALIZER_LIST(weak_ptr_factory_(this)) {
  render_view->GetWebView()->setAutofillClient(this);
}

AutofillAgent::~AutofillAgent() {
}

bool AutofillAgent::InputElementClicked(const WebInputElement& element,
                                        bool was_focused,
                                        bool is_focused) {
  if (was_focused)
    ShowSuggestions(element, true, false, true);

  return false;
}

bool AutofillAgent::InputElementLostFocus() {
  return false;
}
 
void AutofillAgent::didAcceptAutofillSuggestion(const WebNode& node,
                                                const WebString& value,
                                                const WebString& label,
                                                int item_id,
                                                unsigned index) {
  DCHECK(node == element_);

  switch (item_id) {
    case WebAutofillClient::MenuItemIDAutocompleteEntry:
    case WebAutofillClient::MenuItemIDPasswordEntry:
      // User selected an Autocomplete or password entry, so we fill directly.
      SetNodeText(value, &element_);
      break;
    case WebAutofillClient::MenuItemIDDataListEntry:
      AcceptDataListSuggestion(value);
      break;
  }
}

void AutofillAgent::didSelectAutofillSuggestion(const WebNode& node,
                                                const WebString& value,
                                                const WebString& label,
                                                int item_id) {
  didClearAutofillSelection(node);
}

void AutofillAgent::didClearAutofillSelection(const WebNode& node) {
}

void AutofillAgent::removeAutocompleteSuggestion(const WebString& name,
                                                 const WebString& value) {
}

void AutofillAgent::textFieldDidEndEditing(const WebInputElement& element) {
}

void AutofillAgent::textFieldDidChange(const WebInputElement& element) {
  if (did_set_node_text_) {
      did_set_node_text_ = false;
      return;
  }

  // We post a task for doing the Autofill as the caret position is not set
  // properly at this point (http://bugs.webkit.org/show_bug.cgi?id=16976) and
  // it is needed to trigger autofill.
  weak_ptr_factory_.InvalidateWeakPtrs();
  MessageLoop::current()->PostTask(
        FROM_HERE,
        base::Bind(&AutofillAgent::TextFieldDidChangeImpl,
                   weak_ptr_factory_.GetWeakPtr(), element));
}

void AutofillAgent::TextFieldDidChangeImpl(const WebInputElement& element) {
  ShowSuggestions(element, false, true, false);
}

void AutofillAgent::textFieldDidReceiveKeyDown(const WebInputElement& element,
                                               const WebKeyboardEvent& event) {
  if (event.windowsKeyCode == ui::VKEY_DOWN ||
      event.windowsKeyCode == ui::VKEY_UP)
    ShowSuggestions(element, true, true, true);
}

void AutofillAgent::ShowSuggestions(const WebInputElement& element,
                                    bool autofill_on_empty_values,
                                    bool requires_caret_at_end,
                                    bool display_warning_if_disabled) {
  if (!element.isEnabled() || element.isReadOnly() || !element.isTextField() ||
      element.isPasswordField() || !element.suggestedValue().isEmpty())
    return;

  // Don't attempt to autofill with values that are too large or if filling
  // criteria are not met.
  WebString value = element.editingValue();
  if (value.length() > kMaximumTextSizeForAutofill ||
      (!autofill_on_empty_values && value.isEmpty()) ||
      (requires_caret_at_end &&
       (element.selectionStart() != element.selectionEnd() ||
        element.selectionEnd() != static_cast<int>(value.length())))) {
    // Any popup currently showing is obsolete.
    WebKit::WebView* web_view = render_view()->GetWebView();
    if (web_view)
      web_view->hidePopups();

    return;
  }

  element_ = element;

  // TODO: Currently node-webkit didn't support HTML5 attribute |autocomplete|
  // of the |input|.
  std::vector<string16> v;
  std::vector<string16> l;
  std::vector<string16> i;
  std::vector<int> ids;
  AppendDataListSuggestions(element, &v, &l, &i, &ids);

  WebKit::WebView* web_view = render_view()->GetWebView();
  if (!web_view)
    return;

  if (v.empty()) {
    // No suggestions, any popup currently showing is obsolete.
    web_view->hidePopups();
    return;
  }

  // Send to WebKit for display.
  web_view->applyAutofillSuggestions(element, v, l, i, ids);
}

void AutofillAgent::AcceptDataListSuggestion(const string16& suggested_value) {
  string16 new_value = suggested_value;
  // If this element takes multiple values then replace the last part with
  // the suggestion.
  if (element_.isMultiple() &&
      element_.formControlType() == WebString::fromUTF8("email")) {
    std::vector<string16> parts;

    base::SplitStringDontTrim(element_.editingValue(), ',', &parts);
    if (parts.size() == 0)
      parts.push_back(string16());

    string16 last_part = parts.back();
    // We want to keep just the leading whitespace.
    for (size_t i = 0; i < last_part.size(); ++i) {
      if (!IsWhitespace(last_part[i])) {
        last_part = last_part.substr(0, i);
        break;
      }
    }
    last_part.append(suggested_value);
    parts[parts.size() - 1] = last_part;

    new_value = JoinString(parts, ',');
  }
  SetNodeText(new_value, &element_);
}

void AutofillAgent::SetNodeText(const string16& value,
                                WebKit::WebInputElement* node) {
  did_set_node_text_ = true;
  string16 substring = value;
  substring = substring.substr(0, node->maxLength());

  node->setEditingValue(substring);
}
 
}  // namespace nw
