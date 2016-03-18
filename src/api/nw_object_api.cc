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

NwObjCreateFunction::NwObjCreateFunction() {
}

NwObjCreateFunction::~NwObjCreateFunction() {
}

bool NwObjCreateFunction::RunNWSync(base::ListValue* response, std::string* error) {
  base::DictionaryValue* options = nullptr;
  int id = 0;
  std::string type;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));
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
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnDeallocateObject(id);
  return true;
}

NwObjCallObjectMethodFunction::NwObjCallObjectMethodFunction() {
}

NwObjCallObjectMethodFunction::~NwObjCallObjectMethodFunction() {
}

bool NwObjCallObjectMethodFunction::RunNWSync(base::ListValue* response, std::string* error) {
  base::ListValue* arguments = nullptr;
  int id = 0;
  std::string type, method;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &type));
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(2, &method));
  EXTENSION_FUNCTION_VALIDATE(args_->GetList(3, &arguments));

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethod(render_frame_host(), id, type, method, *arguments);
  return true;
}

NwObjCallObjectMethodSyncFunction::NwObjCallObjectMethodSyncFunction() {
}

NwObjCallObjectMethodSyncFunction::~NwObjCallObjectMethodSyncFunction() {
}

bool NwObjCallObjectMethodSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  base::ListValue* arguments = nullptr;
  int id = 0;
  std::string type, method;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &type));
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(2, &method));
  EXTENSION_FUNCTION_VALIDATE(args_->GetList(3, &arguments));

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethodSync(render_frame_host(), id, type, method, *arguments, response);
  return true;
}

NwObjCallObjectMethodAsyncFunction::NwObjCallObjectMethodAsyncFunction() {
}

NwObjCallObjectMethodAsyncFunction::~NwObjCallObjectMethodAsyncFunction() {
}

bool NwObjCallObjectMethodAsyncFunction::RunAsync() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  base::ListValue* arguments = nullptr;
  int id = 0;
  std::string type, method;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &type));
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(2, &method));
  EXTENSION_FUNCTION_VALIDATE(args_->GetList(3, &arguments));

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallObjectMethod(render_frame_host(), id, type, method, *arguments);
  SendResponse(true);
  return true;
}


} // namespace extensions
