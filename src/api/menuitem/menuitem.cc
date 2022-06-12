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

#include "content/nw/src/api/menuitem/menuitem.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/values.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menu/menu.h"

#include <string.h>

namespace nw {

namespace {

typedef std::map<std::string,std::string> KeyMap;

static KeyMap keymap = {
  {"`"    , "Backquote"},
  {"\\"   , "Backslash"},
  {"["    , "BracketLeft"},
  {"]"    , "BracketRight"},
  {","    , "Comma"},
  {"="    , "Equal"},
  {"-"    , "Minus"},
  {"."    , "Period"},
  {"'"    , "Quote"},
  {";"    , "Semicolon"},
  {"/"    , "Slash"},
  {"\n"   , "Enter"},
  {"\t"   , "Tab"},
  {"UP"   , "ArrowUp"},
  {"DOWN" , "ArrowDown"},
  {"LEFT" , "ArrowLeft"},
  {"RIGHT", "ArrowRight"},
  {"ESC"  , "Escape"},
  {"MEDIANEXTTRACK", "MediaTrackNext"},
  {"MEDIAPREVTRACK", "MediaTrackPrevious"}
};

}

ui::KeyboardCode GetKeycodeFromText(std::string text){
  ui::KeyboardCode retval = ui::VKEY_UNKNOWN;
  if (text.size() != 0){
    std::string upperText = base::ToUpperASCII(text);
    std::string keyName = text;
    bool found = false;
    if (upperText.size() == 1){
      char key = upperText[0];
      if (key>='0' && key<='9'){//handle digital
        keyName = "Digit" + upperText;
        found = true;
      } else if (key>='A'&&key<='Z'){//handle alphabet
        keyName = "Key" + upperText;
        found = true;
      }
    }

    if (!found) {
      KeyMap::iterator it = keymap.find(upperText);
      if (it != keymap.end()) {
        keyName = it->second;
        found = true;
      }
    }

    // build keyboard code
    ui::DomCode domCode = ui::KeycodeConverter::CodeStringToDomCode(keyName.c_str());
    retval = ui::DomCodeToUsLayoutKeyboardCode(domCode);
  }
  return retval;
}

MenuItem::MenuItem(int id,
                   const base::WeakPtr<ObjectManager>& object_manager,
                   const base::DictionaryValue& option,
                   const std::string& extension_id)
  : Base(id, object_manager, option, extension_id) {
  Create(option);
}

MenuItem::~MenuItem() {
  Destroy();
}

void MenuItem::Call(const std::string& method,
                    const base::ListValue& arguments,
                    content::RenderFrameHost* rvh) {
  if (method == "SetLabel") {
    std::string label;
    label = arguments.GetListDeprecated()[0].GetString();
    SetLabel(label);
  } else if (method == "SetIcon") {
    std::string icon;
    icon = arguments.GetListDeprecated()[0].GetString();
    SetIcon(icon);
  } else if (method == "SetIconIsTemplate") {
    bool isTemplate = arguments.GetListDeprecated()[0].GetBool();
    SetIconIsTemplate(isTemplate);
  } else if (method == "SetTooltip") {
    std::string tooltip = arguments.GetListDeprecated()[0].GetString();
    SetTooltip(tooltip);
  } else if (method == "SetEnabled") {
    bool enabled = true;
    enabled = arguments.GetListDeprecated()[0].GetBool();
    SetEnabled(enabled);
  } else if (method == "SetChecked") {
    bool checked = false;
    checked = arguments.GetListDeprecated()[0].GetBool();
    SetChecked(checked);
  } else if (method == "SetSubmenu") {
    int object_id = arguments.GetListDeprecated()[0].GetInt();
    SetSubmenu(object_manager()->GetApiObject<Menu>(object_id));
#if defined(OS_MAC)
  } else if (method == "SetKey") {
    std::string key = arguments.GetList()[0].GetString();
    SetKey(key);
  } else if (method == "SetModifiers") {
    std::string mod = arguments.GetList()[0].GetString();
    SetModifiers(mod);
#endif
  } else {
    NOTREACHED() << "Invalid call to MenuItem method:" << method
                 << " arguments:" << arguments;
  }
}

void MenuItem::CallSync(const std::string& method,
                        const base::Value::List& arguments,
                        base::Value::List* result) {
  if (method == "GetChecked") {
    result->Append(GetChecked());
  } else {
    NOTREACHED() << "Invalid call to MenuItem method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace nwapi
