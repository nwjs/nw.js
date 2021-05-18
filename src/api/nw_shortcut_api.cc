#include "nw_shortcut_api.h"
#include "base/lazy_instance.h"
#include "base/values.h"
#include "chrome/browser/extensions/global_shortcut_listener.h"
#include "content/nw/src/api/nw_shortcut.h"
#include "extensions/browser/extensions_browser_client.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/events/keycodes/keyboard_code_conversion.h"

using namespace extensions::nwapi::nw__shortcut;
using namespace content;

namespace extensions {

class NWShortcutObserver : public GlobalShortcutListener::Observer {
public:
  static NWShortcutObserver* GetInstance();

  // ui::GlobalShortcutListener::Observer implementation.
  void OnKeyPressed(const ui::Accelerator& uiAccelerator) override;

};

namespace {

  std::unique_ptr<ui::Accelerator> dictionaryToUIAccelerator(const base::DictionaryValue *acceleratorDict) {
    nwapi::nw__shortcut::Accelerator accelerator;
    nwapi::nw__shortcut::Accelerator::Populate(*acceleratorDict, &accelerator);

    // build keyboard code
    ui::DomCode domCode = ui::KeycodeConverter::CodeStringToDomCode(accelerator.key.c_str());
    ui::KeyboardCode keyboardCode = ui::DomCodeToUsLayoutKeyboardCode(domCode);

    // build modifier
    int modifiers = ui::EF_NONE;
    if (accelerator.modifiers.alt) {
      modifiers |= ui::EF_ALT_DOWN;
    }
    if (accelerator.modifiers.command) {
      modifiers |= ui::EF_COMMAND_DOWN;
    }
    if (accelerator.modifiers.ctrl) {
      modifiers |= ui::EF_CONTROL_DOWN;
    }
    if (accelerator.modifiers.shift) {
      modifiers |= ui::EF_SHIFT_DOWN;
    }

    std::unique_ptr<ui::Accelerator> uiAccelerator(new ui::Accelerator(keyboardCode, modifiers));

    return uiAccelerator;
  }

  std::unique_ptr<nwapi::nw__shortcut::Accelerator> UIAcceleratorToAccelerator(const ui::Accelerator &uiAccelerator) {
    std::unique_ptr<nwapi::nw__shortcut::Accelerator> accelerator(new Accelerator());
    ui::DomCode domCode = ui::UsLayoutKeyboardCodeToDomCode(uiAccelerator.key_code());
    accelerator->key = ui::KeycodeConverter::DomCodeToCodeString(domCode);
    int modifiers = uiAccelerator.modifiers();
    if (modifiers & ui::EF_ALT_DOWN) {
      accelerator->modifiers.alt = true;
    }
    if (modifiers & ui::EF_COMMAND_DOWN) {
      accelerator->modifiers.command = true;
    }
    if (modifiers & ui::EF_CONTROL_DOWN) {
      accelerator->modifiers.ctrl = true;
    }
    if (modifiers & ui::EF_SHIFT_DOWN) {
      accelerator->modifiers.shift = true;
    }
    return accelerator;
  }

  void DispatchEvent(
      events::HistogramValue histogram_value,
      const std::string& event_name,
      std::vector<base::Value> args) {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    ExtensionsBrowserClient::Get()->
      BroadcastEventToRenderers(
                                histogram_value, event_name,
                                std::make_unique<base::ListValue>(std::move(args)),
                                false);
  }

  base::LazyInstance<NWShortcutObserver>::Leaky
    g_nw_shortcut_observer_ = LAZY_INSTANCE_INITIALIZER;

} //

// static
NWShortcutObserver* NWShortcutObserver::GetInstance() {
  return g_nw_shortcut_observer_.Pointer();
}

void NWShortcutObserver::OnKeyPressed (const ui::Accelerator& uiAccelerator) {
  std::unique_ptr<nwapi::nw__shortcut::Accelerator> accelerator = UIAcceleratorToAccelerator(uiAccelerator);
  std::vector<base::Value> args =
    nwapi::nw__shortcut::OnKeyPressed::Create(*accelerator);
  DispatchEvent(
    events::HistogramValue::UNKNOWN,
    nwapi::nw__shortcut::OnKeyPressed::kEventName,
    std::move(args));
}

bool NwShortcutRegisterAcceleratorFunction::RunNWSync(base::ListValue* response, std::string* error) {
  const base::DictionaryValue *acceleratorDict = nullptr;
  EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &acceleratorDict));
  std::unique_ptr<ui::Accelerator> uiAccelerator = dictionaryToUIAccelerator(acceleratorDict);

  if (!GlobalShortcutListener::GetInstance()->RegisterAccelerator(*uiAccelerator, NWShortcutObserver::GetInstance())) {
    response->AppendBoolean(false);
    return true;
  }

  response->AppendBoolean(true);
  return true;
}

bool NwShortcutUnregisterAcceleratorFunction::RunNWSync(base::ListValue* response, std::string* error) {
  const base::DictionaryValue *acceleratorDict = nullptr;
  EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &acceleratorDict));
  std::unique_ptr<ui::Accelerator> uiAccelerator = dictionaryToUIAccelerator(acceleratorDict);

  GlobalShortcutListener::GetInstance()->UnregisterAccelerator(*uiAccelerator, NWShortcutObserver::GetInstance());
  response->AppendBoolean(true);
  return true;
}

} // extensions
