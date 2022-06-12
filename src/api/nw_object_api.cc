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

bool NwObjAllocateIdFunction::RunNWSync(base::Value::List* response, std::string* error) {
  response->Append(nw::ObjectManager::AllocateId());
  return true;
}

NwObjCreateFunction::NwObjCreateFunction() {
}

NwObjCreateFunction::~NwObjCreateFunction() {
}

bool NwObjCreateFunction::RunNWSync(base::Value::List* response, std::string* error) {
  int id = 0;
  std::string type;

  EXTENSION_FUNCTION_VALIDATE(args().size() >= 3);
  EXTENSION_FUNCTION_VALIDATE(args()[0].is_int() && args()[1].is_string() &&
                              args()[2].is_dict());
  id = args()[0].GetInt();
  type = args()[1].GetString();
  const base::DictionaryValue& options = base::Value::AsDictionaryValue(args()[2]);
  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnAllocateObject(id, type, options, extension_id());
  return true;
}

NwObjDestroyFunction::NwObjDestroyFunction() {
}

NwObjDestroyFunction::~NwObjDestroyFunction() {
}

bool NwObjDestroyFunction::RunNWSync(base::Value::List* response, std::string* error) {
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 1);
  EXTENSION_FUNCTION_VALIDATE(args()[0].is_int());
  int id = args()[0].GetInt();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnDeallocateObject(id);
  return true;
}

NwObjCallObjectMethodFunction::NwObjCallObjectMethodFunction() {
}

NwObjCallObjectMethodFunction::~NwObjCallObjectMethodFunction() {
}

bool NwObjCallObjectMethodFunction::RunNWSync(base::Value::List* response, std::string* error) {
  int id = 0;
  std::string type, method;
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 4 &&
                              args()[0].is_int() &&
                              args()[1].is_string() &&
                              args()[2].is_string() &&
                              args()[3].is_list());
  id = args()[0].GetInt();
  type = args()[1].GetString();
  method = args()[2].GetString();
  base::Value::ConstListView arguments = args()[3].GetListDeprecated();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethod(render_frame_host(), id, type, method, base::ListValue(arguments));
  return true;
}

NwObjCallObjectMethodSyncFunction::NwObjCallObjectMethodSyncFunction() {
}

NwObjCallObjectMethodSyncFunction::~NwObjCallObjectMethodSyncFunction() {
}

bool NwObjCallObjectMethodSyncFunction::RunNWSync(base::Value::List* response, std::string* error) {
  int id = 0;
  std::string type, method;
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 4 &&
                              args()[0].is_int() &&
                              args()[1].is_string() &&
                              args()[2].is_string() &&
                              args()[3].is_list());
  id = args()[0].GetInt();
  type = args()[1].GetString();
  method = args()[2].GetString();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethodSync(render_frame_host(), id, type, method, args()[3].GetList(), response);
  return true;
}

NwObjCallObjectMethodAsyncFunction::NwObjCallObjectMethodAsyncFunction() {
}

NwObjCallObjectMethodAsyncFunction::~NwObjCallObjectMethodAsyncFunction() {
}

ExtensionFunction::ResponseAction
NwObjCallObjectMethodAsyncFunction::Run() {
  int id = 0;
  std::string type, method;
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 4 &&
                              args()[0].is_int() &&
                              args()[1].is_string() &&
                              args()[2].is_string() &&
                              args()[3].is_list());
  id = args()[0].GetInt();
  type = args()[1].GetString();
  method = args()[2].GetString();
  base::Value::ConstListView arguments = args()[3].GetListDeprecated();

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethod(render_frame_host(), id, type, method, base::ListValue(arguments));
  return RespondNow(NoArguments());
}


} // namespace extensions
