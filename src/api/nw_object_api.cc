#include "content/nw/src/api/nw_object_api.h"

#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/api/object_manager.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"

namespace extensions {

NwObjAllocateIdFunction::NwObjAllocateIdFunction() {
}

NwObjAllocateIdFunction::~NwObjAllocateIdFunction() {
}

bool NwObjAllocateIdFunction::RunNWSync(base::ListValue* response, std::string* error) {
  response->AppendInteger(nw::ObjectManager::AllocateId());
  return true;
}

NwObjCreateFunction::NwObjCreateFunction() {
}

NwObjCreateFunction::~NwObjCreateFunction() {
}

bool NwObjCreateFunction::RunNWSync(base::ListValue* response, std::string* error) {
  base::DictionaryValue* options = nullptr;
  int id = 0;
  std::string type;
  base::Value::ConstListView list_view = args_->GetList();
  EXTENSION_FUNCTION_VALIDATE(list_view.size() > 0 && list_view[0].is_int());
  id = list_view[0].GetInt();
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &type));
  EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(2, &options));

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnAllocateObject(id, type, *options, extension_id());
  return true;
}

NwObjDestroyFunction::NwObjDestroyFunction() {
}

NwObjDestroyFunction::~NwObjDestroyFunction() {
}

bool NwObjDestroyFunction::RunNWSync(base::ListValue* response, std::string* error) {
  int id = 0;
  base::Value::ConstListView list_view = args_->GetList();
  EXTENSION_FUNCTION_VALIDATE(list_view.size() > 0 && list_view[0].is_int());
  id = list_view[0].GetInt();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnDeallocateObject(id);
  return true;
}

NwObjCallObjectMethodFunction::NwObjCallObjectMethodFunction() {
}

NwObjCallObjectMethodFunction::~NwObjCallObjectMethodFunction() {
}

bool NwObjCallObjectMethodFunction::RunNWSync(base::ListValue* response, std::string* error) {
  int id = 0;
  std::string type, method;
  base::Value::ConstListView args_list = args_->GetList();
  EXTENSION_FUNCTION_VALIDATE(!args_list.empty() &&
                              args_list[0].is_int() &&
                              args_list[1].is_string() &&
                              args_list[2].is_string() &&
                              args_list[3].is_list());
  id = args_list[0].GetInt();
  type = args_list[1].GetString();
  method = args_list[2].GetString();
  base::Value::ConstListView arguments = args_list[3].GetList();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethod(render_frame_host(), id, type, method, base::ListValue(arguments));
  return true;
}

NwObjCallObjectMethodSyncFunction::NwObjCallObjectMethodSyncFunction() {
}

NwObjCallObjectMethodSyncFunction::~NwObjCallObjectMethodSyncFunction() {
}

bool NwObjCallObjectMethodSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  int id = 0;
  std::string type, method;
  base::Value::ConstListView args_list = args_->GetList();
  EXTENSION_FUNCTION_VALIDATE(!args_list.empty() &&
                              args_list[0].is_int() &&
                              args_list[1].is_string() &&
                              args_list[2].is_string() &&
                              args_list[3].is_list());
  id = args_list[0].GetInt();
  type = args_list[1].GetString();
  method = args_list[2].GetString();
  base::Value::ConstListView arguments = args_list[3].GetList();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethodSync(render_frame_host(), id, type, method, base::ListValue(arguments), response);
  return true;
}

NwObjCallObjectMethodAsyncFunction::NwObjCallObjectMethodAsyncFunction() {
}

NwObjCallObjectMethodAsyncFunction::~NwObjCallObjectMethodAsyncFunction() {
}

ExtensionFunction::ResponseAction
NwObjCallObjectMethodAsyncFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  int id = 0;
  std::string type, method;
  base::Value::ConstListView args_list = args_->GetList();
  EXTENSION_FUNCTION_VALIDATE(!args_list.empty() &&
                              args_list[0].is_int() &&
                              args_list[1].is_string() &&
                              args_list[2].is_string() &&
                              args_list[3].is_list());
  id = args_list[0].GetInt();
  type = args_list[1].GetString();
  method = args_list[2].GetString();
  base::Value::ConstListView arguments = args_list[3].GetList();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethod(render_frame_host(), id, type, method, base::ListValue(arguments));
  return RespondNow(NoArguments());
}


} // namespace extensions
