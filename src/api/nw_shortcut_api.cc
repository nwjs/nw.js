#include "nw_shortcut_api.h"
#include "base/lazy_instance.h"
#include "base/values.h"
#include "content/nw/src/api/nw_shortcut.h"
#include "extensions/browser/extensions_browser_client.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/accelerators/global_accelerator_listener/global_accelerator_listener.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/events/keycodes/keyboard_code_conversion.h"

using namespace extensions::nwapi::nw__shortcut;
using namespace content;

namespace extensions {

class NWShortcutObserver : public ui::GlobalAcceleratorListener::Observer {
public:
  static NWShortcutObserver* GetInstance();

  // ui::GlobalShortcutListener::Observer implementation.
  void OnKeyPressed(const ui::Accelerator& uiAccelerator) override;
  void ExecuteCommand(const ExtensionId& extension_id,
                      const std::string& command_id) override;
};

namespace {

std::unique_ptr<ui::Accelerator> dictionaryToUIAccelerator(const base::Value::Dict *acceleratorDict) {
    nwapi::nw__shortcut::Accelerator accelerator;
    nwapi::nw__shortcut::Accelerator::Populate(acceleratorDict->Clone(), accelerator);

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
      base::Value::List args) {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    base::Value::List event_args;
    for (size_t i = 0; i < args.size(); i++) {
      event_args.Append(args[i].Clone());
    }

    ExtensionsBrowserClient::Get()->
      BroadcastEventToRenderers(histogram_value, event_name, std::move(event_args), false);
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
  auto args(nwapi::nw__shortcut::OnKeyPressed::Create(*accelerator));
  DispatchEvent(
    events::HistogramValue::UNKNOWN,
    nwapi::nw__shortcut::OnKeyPressed::kEventName,
    std::move(args));
}

void NWShortcutObserver::ExecuteCommand(
    const ExtensionId& extension_id,
    const std::string& command_id) {
}

bool NwShortcutRegisterAcceleratorFunction::RunNWSync(base::Value::List* response, std::string* error) {
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 1);
  EXTENSION_FUNCTION_VALIDATE(args()[0].is_dict());
  const base::Value::Dict& acceleratorDict = args()[0].GetDict();

  std::unique_ptr<ui::Accelerator> uiAccelerator = dictionaryToUIAccelerator(&acceleratorDict);

  if (!ui::GlobalAcceleratorListener::GetInstance()->RegisterAccelerator(*uiAccelerator, NWShortcutObserver::GetInstance())) {
    response->Append(false);
    return true;
  }

  response->Append(true);
  return true;
}

bool NwShortcutUnregisterAcceleratorFunction::RunNWSync(base::Value::List* response, std::string* error) {
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 1);
  EXTENSION_FUNCTION_VALIDATE(args()[0].is_dict());
  const base::Value::Dict& acceleratorDict = args()[0].GetDict();
  std::unique_ptr<ui::Accelerator> uiAccelerator = dictionaryToUIAccelerator(&acceleratorDict);

  ui::GlobalAcceleratorListener::GetInstance()->UnregisterAccelerator(*uiAccelerator, NWShortcutObserver::GetInstance());
  response->Append(true);
  return true;
}

} // extensions
