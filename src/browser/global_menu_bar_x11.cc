// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/global_menu_bar_x11.h"

#include <dlfcn.h>
#include <glib-object.h>
#include <stddef.h>

#include "base/debug/leak_annotations.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host_x11.h"
#include "content/nw/src/browser/global_menu_bar_registrar_x11.h"
#include "chrome/grit/generated_resources.h"
// #include "grit/components_strings.h"
#include "ui/base/accelerators/menu_label_accelerator_util_linux.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/events/keycodes/keyboard_code_conversion_x.h"
#include "ui/gfx/text_elider.h"
#include "ui/base/models/menu_model.h"

#define DLSYM_DBUSMENU_FUNC(name) \
  name = \
      reinterpret_cast<dbusmenu_ ## name ## _func>( \
          dlsym(dbusmenu_lib, "dbusmenu_" # name)); \
  if (!name) return false;

namespace nw {

// libdbusmenu-glib types
typedef struct _DbusmenuMenuitem DbusmenuMenuitem;
typedef DbusmenuMenuitem* (*dbusmenu_menuitem_new_func)();
typedef bool (*dbusmenu_menuitem_child_add_position_func)(
    DbusmenuMenuitem* parent,
    DbusmenuMenuitem* child,
    unsigned int position);
typedef DbusmenuMenuitem* (*dbusmenu_menuitem_child_append_func)(
    DbusmenuMenuitem* parent,
    DbusmenuMenuitem* child);
typedef bool (*dbusmenu_menuitem_child_delete_func)(
    DbusmenuMenuitem* parent,
    DbusmenuMenuitem* child);
typedef GList* (*dbusmenu_menuitem_get_children_func)(
    DbusmenuMenuitem* item);
typedef DbusmenuMenuitem* (*dbusmenu_menuitem_property_set_func)(
    DbusmenuMenuitem* item,
    const char* property,
    const char* value);
typedef DbusmenuMenuitem* (*dbusmenu_menuitem_property_set_variant_func)(
    DbusmenuMenuitem* item,
    const char* property,
    GVariant* value);
typedef DbusmenuMenuitem* (*dbusmenu_menuitem_property_set_bool_func)(
    DbusmenuMenuitem* item,
    const char* property,
    bool value);
typedef DbusmenuMenuitem* (*dbusmenu_menuitem_property_set_int_func)(
    DbusmenuMenuitem* item,
    const char* property,
    int value);

typedef struct _DbusmenuServer      DbusmenuServer;
typedef DbusmenuServer* (*dbusmenu_server_new_func)(const char* object);
typedef void (*dbusmenu_server_set_root_func)(DbusmenuServer* self,
                                              DbusmenuMenuitem* root);

namespace {

// Retrieved functions from libdbusmenu-glib.

// DbusmenuMenuItem methods:
dbusmenu_menuitem_new_func menuitem_new = nullptr;
dbusmenu_menuitem_get_children_func menuitem_get_children = nullptr;
dbusmenu_menuitem_child_add_position_func menuitem_child_add_position = nullptr;
dbusmenu_menuitem_child_append_func menuitem_child_append = nullptr;
dbusmenu_menuitem_child_delete_func menuitem_child_delete = nullptr;
dbusmenu_menuitem_property_set_func menuitem_property_set = nullptr;
dbusmenu_menuitem_property_set_variant_func menuitem_property_set_variant =
    nullptr;
dbusmenu_menuitem_property_set_bool_func menuitem_property_set_bool = nullptr;
dbusmenu_menuitem_property_set_int_func menuitem_property_set_int = nullptr;

// DbusmenuServer methods:
dbusmenu_server_new_func server_new = nullptr;
dbusmenu_server_set_root_func server_set_root = nullptr;

// Properties that we set on menu items:
const char kPropertyEnabled[] = "enabled";
const char kPropertyLabel[] = "label";
const char kPropertyShortcut[] = "shortcut";
const char kPropertyType[] = "type";
const char kPropertyToggleType[] = "toggle-type";
const char kPropertyToggleState[] = "toggle-state";
const char kPropertyUseUnderline[] = "use-underline";
const char kPropertyVisible[] = "visible";
const char kPropertyChildrenDisplay[] = "children-display";

const char kTypeCheckmark[] = "checkmark";
const char kTypeSeparator[] = "separator";
const char kTypeSubmenu[] = "submenu";

// Data set on GObjectgs.
const char kTypeTag[] = "type-tag";

// static
bool EnsureMethodsLoaded() {
  static bool attempted_load = false;
  if (attempted_load)
    return true;

  void* dbusmenu_lib = dlopen("libdbusmenu-glib.so", RTLD_LAZY);
  if (!dbusmenu_lib)
    dbusmenu_lib = dlopen("libdbusmenu-glib.so.4", RTLD_LAZY);
  if (!dbusmenu_lib)
    return false;

  // DbusmenuMenuItem methods.
  DLSYM_DBUSMENU_FUNC(menuitem_new)
  DLSYM_DBUSMENU_FUNC(menuitem_child_add_position)
  DLSYM_DBUSMENU_FUNC(menuitem_child_append)
  DLSYM_DBUSMENU_FUNC(menuitem_child_delete)
  DLSYM_DBUSMENU_FUNC(menuitem_get_children)
  DLSYM_DBUSMENU_FUNC(menuitem_property_set)
  DLSYM_DBUSMENU_FUNC(menuitem_property_set_variant)
  DLSYM_DBUSMENU_FUNC(menuitem_property_set_bool)
  DLSYM_DBUSMENU_FUNC(menuitem_property_set_int)

  // DbusmenuServer methods.
  DLSYM_DBUSMENU_FUNC(server_new)
  DLSYM_DBUSMENU_FUNC(server_set_root)

  attempted_load = true;

  return true;
}

}  // namespace

GlobalMenuBarX11::GlobalMenuBarX11(views::DesktopWindowTreeHostX11* host, ui::MenuModel* model)
    : host_(host),
      server_(nullptr),
      root_item_(nullptr),
      model_(model),
      weak_ptr_factory_(this) {
  EnsureMethodsLoaded();

  if (server_new) {
    host_->AddObserver(this);
    OnWindowMapped(host_->GetAcceleratedWidget());
  }
}

GlobalMenuBarX11::~GlobalMenuBarX11() {
  if (server_) {
    g_object_unref(server_);
    host_->RemoveObserver(this);
    OnWindowUnmapped(host_->GetAcceleratedWidget());
  }
}

// static
std::string GlobalMenuBarX11::GetPathForWindow(unsigned long xid) {
  return base::StringPrintf("/com/canonical/menu/%lX", xid);
}

// static
bool GlobalMenuBarX11::IsGlobalMenuBarEnabled() {
  return EnsureMethodsLoaded();
}

DbusmenuMenuitem* GlobalMenuBarX11::BuildSeparator() {
  DbusmenuMenuitem* item = menuitem_new();
  menuitem_property_set(item, kPropertyType, kTypeSeparator);
  menuitem_property_set_bool(item, kPropertyVisible, true);
  return item;
}

DbusmenuMenuitem* GlobalMenuBarX11::BuildMenuItem(
    const std::string& label,
    int tag_id) {
  DbusmenuMenuitem* item = menuitem_new();
  menuitem_property_set(item, kPropertyLabel, label.c_str());
  menuitem_property_set_bool(item, kPropertyVisible, true);
  menuitem_property_set_bool(item, kPropertyUseUnderline, true);

  if (tag_id)
    g_object_set_data(G_OBJECT(item), kTypeTag, GINT_TO_POINTER(tag_id));

  return item;
}

void GlobalMenuBarX11::InitServer(unsigned long xid) {
  std::string path = GetPathForWindow(xid);
  {
    ANNOTATE_SCOPED_MEMORY_LEAK; // http://crbug.com/314087
    server_ = server_new(path.c_str());
  }

  root_item_ = menuitem_new();
  menuitem_property_set(root_item_, kPropertyLabel, "Root");
  menuitem_property_set_bool(root_item_, kPropertyVisible, true);
  menuitem_property_set_bool(root_item_, kPropertyUseUnderline, true);

  BuildStaticMenuFromModel(root_item_, model_);

  server_set_root(server_, root_item_);
}

DbusmenuMenuitem* GlobalMenuBarX11::BuildStaticMenuFromModel(
    DbusmenuMenuitem* parent, ui::MenuModel* model) {

  for (int i = 0; i < model->GetItemCount(); ++i) {
    DbusmenuMenuitem* menu_item = nullptr;
    auto type = model->GetTypeAt(i);
    if (type == ui::MenuModel::TYPE_SEPARATOR) {
      menu_item = BuildSeparator();
    } else {
      int command_id = model->GetCommandIdAt(i);
      std::string label = ui::ConvertAcceleratorsFromWindowsStyle(
          base::UTF16ToUTF8(model->GetLabelAt(i)));

      menu_item = BuildMenuItem(label, 0);

      if (!model->IsEnabledAt(i)) {
        menuitem_property_set_bool(menu_item, kPropertyEnabled, false);
      } else {
        if (type == ui::MenuModel::TYPE_CHECK) {
          menuitem_property_set(menu_item, kPropertyToggleType, kTypeCheckmark);
          menuitem_property_set_int(menu_item, kPropertyToggleState, model->IsItemCheckedAt(i));
        } else if (type == ui::MenuModel::TYPE_SUBMENU) {
          menuitem_property_set(menu_item, kPropertyChildrenDisplay, kTypeSubmenu);
          // Submenus on root item should be added once, or it will trigger
          // unnecessary "about-to-show" event.
          if (parent == root_item_)
            BuildStaticMenuFromModel(menu_item, model->GetSubmenuModelAt(i));
          g_signal_connect(menu_item, "about-to-show",
                         G_CALLBACK(OnSubMenuShowThunk), this);
        }

        g_object_set_data(G_OBJECT(menu_item), "command-id",
                          GINT_TO_POINTER(command_id));
        g_signal_connect(menu_item, "item-activated",
                         G_CALLBACK(OnItemActivatedThunk), this);

        ui::Accelerator accelerator;
        if (model->GetAcceleratorAt(i, &accelerator)) {
          RegisterAccelerator(menu_item, accelerator);
        }
      }
    }

    menuitem_child_append(parent, menu_item);
    g_object_unref(menu_item);
  }

  return parent;
}

void GlobalMenuBarX11::RegisterAccelerator(DbusmenuMenuitem* item,
                                           const ui::Accelerator& accelerator) {
  // A translation of libdbusmenu-gtk's menuitem_property_set_shortcut()
  // translated from GDK types to ui::Accelerator types.
  GVariantBuilder builder;
  g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);

  if (accelerator.IsCmdDown())
    g_variant_builder_add(&builder, "s", "Super");
  if (accelerator.IsCtrlDown())
    g_variant_builder_add(&builder, "s", "Control");
  if (accelerator.IsAltDown())
    g_variant_builder_add(&builder, "s", "Alt");
  if (accelerator.IsShiftDown())
    g_variant_builder_add(&builder, "s", "Shift");

  char* name = XKeysymToString(XKeysymForWindowsKeyCode(
      accelerator.key_code(), false));
  if (!name) {
    NOTIMPLEMENTED();
    return;
  }
  g_variant_builder_add(&builder, "s", name);

  GVariant* inside_array = g_variant_builder_end(&builder);
  g_variant_builder_init(&builder, G_VARIANT_TYPE_ARRAY);
  g_variant_builder_add_value(&builder, inside_array);
  GVariant* outside_array = g_variant_builder_end(&builder);

  menuitem_property_set_variant(item, kPropertyShortcut, outside_array);
}

void GlobalMenuBarX11::ClearMenuSection(DbusmenuMenuitem* menu) {
  std::vector<DbusmenuMenuitem*> menuitems_to_delete;

  GList* childs = menuitem_get_children(menu);
  for (; childs != nullptr; childs = childs->next) {
    DbusmenuMenuitem* current_item = reinterpret_cast<DbusmenuMenuitem*>(
        childs->data);
    ClearMenuSection(current_item);

    menuitems_to_delete.push_back(current_item);
  }

  for (std::vector<DbusmenuMenuitem*>::const_iterator it =
           menuitems_to_delete.begin(); it != menuitems_to_delete.end(); ++it) {
    menuitem_child_delete(menu, *it);
  }
}

void GlobalMenuBarX11::OnWindowMapped(unsigned long xid) {
  if (!server_)
    InitServer(xid);

  GlobalMenuBarRegistrarX11::GetInstance()->OnWindowMapped(xid);
}

void GlobalMenuBarX11::OnWindowUnmapped(unsigned long xid) {
  GlobalMenuBarRegistrarX11::GetInstance()->OnWindowUnmapped(xid);
}

void GlobalMenuBarX11::OnItemActivated(DbusmenuMenuitem* item,
                                       unsigned int timestamp) {
  int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(item), "command-id"));
  ui::MenuModel* model = model_;
  int index = -1;
  if (ui::MenuModel::GetModelAndIndexForCommandId(id, &model, &index)) {
    model->ActivatedAt(index);
  }
}

void GlobalMenuBarX11::OnSubMenuShow(DbusmenuMenuitem* item) {
  int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(item), "command-id"));
  ui::MenuModel* model = model_;
  int index = -1;
  if (!ui::MenuModel::GetModelAndIndexForCommandId(id, &model, &index)) {
    return;
  }

  ClearMenuSection(item);

  BuildStaticMenuFromModel(item, model->GetSubmenuModelAt(index));

  model->ActivatedAt(index);
}

} // namespace nw
