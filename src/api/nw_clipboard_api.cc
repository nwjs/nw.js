#include "content/nw/src/api/nw_clipboard_api.h"

#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"
#include "ui/base/clipboard/clipboard.h"

namespace extensions {

NwClipboardGetSyncFunction::NwClipboardGetSyncFunction() {
}

NwClipboardGetSyncFunction::~NwClipboardGetSyncFunction() {
}

bool NwClipboardGetSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {

  ui::Clipboard* clipboard = ui::Clipboard::GetForCurrentThread();
  base::string16 text;
  clipboard->ReadText(ui::CLIPBOARD_TYPE_COPY_PASTE, &text);
  response->Append(new base::StringValue(text));
  LOG(INFO) << "NwClipboardGetSyncFunction::RunSync: " << text;
  return true;
}

NwClipboardSetSyncFunction::NwClipboardSetSyncFunction() {

}

NwClipboardSetSyncFunction::~NwClipboardSetSyncFunction() {
}


bool NwClipboardSetSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  std::string text;
  args_->GetString(0, &text);

  ui::Clipboard* clipboard = ui::Clipboard::GetForCurrentThread();
  ui::Clipboard::ObjectMap map;
  map[ui::Clipboard::CBF_TEXT].push_back(
      std::vector<char>(text.begin(), text.end()));
  clipboard->WriteObjects(ui::CLIPBOARD_TYPE_COPY_PASTE, map);

  return true;
}

NwClipboardClearSyncFunction::NwClipboardClearSyncFunction() {

}

NwClipboardClearSyncFunction::~NwClipboardClearSyncFunction() {

}

bool NwClipboardClearSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  ui::Clipboard* clipboard = ui::Clipboard::GetForCurrentThread();
  clipboard->Clear(ui::CLIPBOARD_TYPE_COPY_PASTE);
  return true;
}

} // namespace extensions
