#include "content/nw/src/api/nw_menu_api.h"
#include "base/strings/sys_string_conversions.h"
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
  for (size_t i = 0; i < base::size(msg_map); i++) {
    g_msgid_map.insert(std::make_pair(msg_map[i].msgstr, msg_map[i].msgid));
  }
  g_msgid_inited = true;
}

bool NwMenuGetNSStringWithFixupFunction::RunNWSync(base::ListValue* response, std::string* error) {
  if (!g_msgid_inited) InitMsgIDMap();
  std::string msgstr;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &msgstr));
  MsgIDMap::iterator it = g_msgid_map.find(msgstr);
  if (it != g_msgid_map.end()) {
    int msgid = it->second;
    response->AppendString(base::SysNSStringToUTF16(l10n_util::GetNSStringWithFixup(msgid)));
    return true;
  }
  return false;
}

bool NwMenuGetNSStringFWithFixupFunction::RunNWSync(base::ListValue* response, std::string* error) {
  if (!g_msgid_inited) InitMsgIDMap();
  std::string msgstr;
  std::u16string appName;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &msgstr));
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &appName));
  MsgIDMap::iterator it = g_msgid_map.find(msgstr);
  if (it != g_msgid_map.end()) {
    int msgid = it->second;
    response->AppendString(base::SysNSStringToUTF16(l10n_util::GetNSStringFWithFixup(msgid, appName)));
    return true;
  }
  return false;
}

} // namespace extensions
