#pragma clang diagnostic ignored "-Wexit-time-destructors"

#include "content/nw/src/api/nw_menu_api.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/grit/generated_resources.h"
#include "ui/base/l10n/l10n_util_mac.h"

namespace extensions {

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

typedef std::unordered_map<std::string, int> MsgIDMap;
MsgIDMap g_msgid_map;
static bool g_msgid_inited = false;

void InitMsgIDMap() {
  g_msgid_map.clear();
  for (size_t i = 0; i < std::size(msg_map); i++) {
    g_msgid_map.insert(std::make_pair(msg_map[i].msgstr, msg_map[i].msgid));
  }
  g_msgid_inited = true;
}

bool NwMenuGetNSStringWithFixupFunction::RunNWSync(base::Value::List* response, std::string* error) {
  if (!g_msgid_inited) InitMsgIDMap();
  EXTENSION_FUNCTION_VALIDATE(args().size() == 1 && args()[0].is_string());
  std::string msgstr = args()[0].GetString();
  MsgIDMap::iterator it = g_msgid_map.find(msgstr);
  if (it != g_msgid_map.end()) {
    int msgid = it->second;
    response->Append(base::SysNSStringToUTF16(l10n_util::GetNSStringWithFixup(msgid)));
    return true;
  }
  return false;
}

bool NwMenuGetNSStringFWithFixupFunction::RunNWSync(base::Value::List* response, std::string* error) {
  if (!g_msgid_inited) InitMsgIDMap();
  EXTENSION_FUNCTION_VALIDATE(args().size() == 2 &&
                              args()[0].is_string() &&
                              args()[1].is_string());
  std::string msgstr = args()[0].GetString();
  std::u16string appName = base::UTF8ToUTF16(args()[1].GetString());
  MsgIDMap::iterator it = g_msgid_map.find(msgstr);
  if (it != g_msgid_map.end()) {
    int msgid = it->second;
    response->Append(base::SysNSStringToUTF16(l10n_util::GetNSStringFWithFixup(msgid, appName)));
    return true;
  }
  return false;
}

} // namespace extensions
