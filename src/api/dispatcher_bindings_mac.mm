#include "content/nw/src/api/dispatcher_bindings.h"

#include <string>

#include "base/containers/hash_tables.h"
#include "grit/nw_strings.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/l10n/l10n_util_mac.h"

#import <Cocoa/Cocoa.h>

namespace nwapi {

typedef struct {
  std::string msgstr;
  int msgid;
} MsgMapEntry;

const MsgMapEntry msg_map[] = {
  { "IDS_ABOUT_MAC", IDS_ABOUT_MAC },
  { "IDS_HIDE_APP_MAC", IDS_HIDE_APP_MAC},
  { "IDS_HIDE_OTHERS_MAC", IDS_HIDE_OTHERS_MAC},
  { "IDS_SHOW_ALL_MAC", IDS_SHOW_ALL_MAC },
  { "IDS_EXIT_MAC", IDS_EXIT_MAC },
  { "IDS_EDIT_MENU_MAC", IDS_EDIT_MENU_MAC },
  { "IDS_EDIT_UNDO_MAC", IDS_EDIT_UNDO_MAC },
  { "IDS_EDIT_REDO_MAC", IDS_EDIT_REDO_MAC },
  { "IDS_CUT_MAC", IDS_CUT_MAC },
  { "IDS_COPY_MAC", IDS_COPY_MAC },
  { "IDS_PASTE_MAC", IDS_PASTE_MAC },
  { "IDS_EDIT_DELETE_MAC", IDS_EDIT_DELETE_MAC },
  { "IDS_EDIT_SELECT_ALL_MAC", IDS_EDIT_SELECT_ALL_MAC },
  { "IDS_WINDOW_MENU_MAC", IDS_WINDOW_MENU_MAC },
  { "IDS_MINIMIZE_WINDOW_MAC", IDS_MINIMIZE_WINDOW_MAC },
  { "IDS_CLOSE_WINDOW_MAC", IDS_CLOSE_WINDOW_MAC },
  { "IDS_ALL_WINDOWS_FRONT_MAC", IDS_ALL_WINDOWS_FRONT_MAC },
};

typedef base::hash_map<std::string, int> MsgIDMap;
MsgIDMap g_msgid_map;

void DispatcherBindings::InitMsgIDMap() {
  g_msgid_map.clear();
  for (size_t i = 0; i < arraysize(msg_map); i++) {
    g_msgid_map.insert(std::make_pair(msg_map[i].msgstr, msg_map[i].msgid));
  }
}

// static
void DispatcherBindings::GetNSStringWithFixup(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  std::string msgstr = *v8::String::Utf8Value(args[0]);
  MsgIDMap::iterator it = g_msgid_map.find(msgstr);
  if (it != g_msgid_map.end()) {
    int msgid = it->second;
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, [l10n_util::GetNSStringWithFixup(msgid) UTF8String]));
    return;
  }
  args.GetReturnValue().Set(v8::Undefined(isolate));
}

// static
void DispatcherBindings::GetNSStringFWithFixup(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  std::string msgstr = *v8::String::Utf8Value(args[0]);
  base::string16 arg = *v8::String::Value(args[1]);
  MsgIDMap::iterator it = g_msgid_map.find(msgstr);
  if (it != g_msgid_map.end()) {
    int msgid = it->second;
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, [l10n_util::GetNSStringFWithFixup(msgid, arg) UTF8String]));
    return;
  }
  args.GetReturnValue().Set(v8::Undefined(isolate));
}

} // namespace nwapi
