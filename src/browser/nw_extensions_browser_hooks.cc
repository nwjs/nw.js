#include "nw_extensions_browser_hooks.h"

#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_operations.h"

// base
#include "base/command_line.h"
#include "base/feature_list.h"
#include "base/json/json_reader.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"

#if defined(OS_WIN)
#include "base/win/scoped_gdi_object.h"
#include "ui/gfx/icon_util.h"
#endif

// content
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/content_features.h"

// content/nw/
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/browser/nw_content_browser_hooks.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_base.h"
#include "content/nw/src/nw_content_verifier_delegate.h"

// extensions
#include "extensions/browser/app_window/app_window.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/guest_view/web_view/web_view_guest.h"
#include "extensions/browser/guest_view/web_view/web_view_renderer_state.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_handlers/webview_info.h"
#include "extensions/common/manifest_url_handlers.h"

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

using content::RenderProcessHost;

using extensions::ContentVerifierDelegate;
using extensions::Extension;
using extensions::ExtensionRegistry;
using extensions::NWContentVerifierDelegate;
using extensions::WebviewInfo;
using extensions::WebViewGuest;

namespace manifest_keys = extensions::manifest_keys;

namespace nw {

namespace {

#if 0
void AmendManifestContentScriptList(base::DictionaryValue* manifest,
                                    const std::string& name,
                                    const std::string& run_at) {
  base::ListValue* scripts = NULL;
  base::Value* temp_value = NULL;
  bool amend = false;

  if (manifest->Get(manifest_keys::kContentScripts, &temp_value))
      if (temp_value->GetAsList(&scripts))
        amend = true;
  if (!scripts)
    scripts = new base::ListValue();

  std::string js;
  if (manifest->GetString(name, &js)) {
    base::ListValue* js_list = new base::ListValue();
    js_list->Append(new base::StringValue(js));

    base::ListValue* matches = new base::ListValue();
    matches->Append(new base::StringValue("<all_urls>"));

    base::ListValue* exclude_matches = new base::ListValue();
    exclude_matches->Append(new base::StringValue("*://*/_generated_background_page.html"));

    base::DictionaryValue* content_script = new base::DictionaryValue();
    content_script->Set("js", js_list);
    content_script->Set("matches", matches);
    content_script->Set("exclude_matches", exclude_matches);
    content_script->SetString("run_at", run_at);
    content_script->SetBoolean("in_main_world", true);

    scripts->Append(content_script);

    if (!amend)
      manifest->Set(manifest_keys::kContentScripts, scripts);
  }
}
#endif

void AmendManifestStringList(base::Value::Dict* manifest,
                   const std::string& path,
                   const std::string& string_value) {
  base::Value::List pattern_list;
  base::Value* temp_pattern_value = nullptr;

  if (path.find('.'))
    temp_pattern_value = manifest->FindByDottedPath(path);
  else
    temp_pattern_value = manifest->Find(path);

  if (temp_pattern_value)
    if (temp_pattern_value->is_list()) {
      pattern_list = temp_pattern_value->GetList().Clone();
    }

  pattern_list.Append(string_value);
  if (path.find('.'))
    manifest->SetByDottedPath(path, std::move(pattern_list));
  else
    manifest->Set(path, std::move(pattern_list));
}

void AmendManifestList(base::Value::Dict* manifest,
		       const std::string& path,
		       const base::Value::List& list_value) {
  base::Value::List* pattern_list = NULL;

  if ((pattern_list = manifest->FindList(path))) {
    for(auto it = list_value.begin(); it != list_value.end(); ++it) {
      pattern_list->Append(it->Clone());
    }
  } else {
    base::Value::List lst;
    for (auto i = list_value.begin(); i != list_value.end(); ++i) {
      lst.Append(const_cast<base::Value&&>(*i));
    }
    if (path.find('.'))
      manifest->SetByDottedPath(path, std::move(lst));
    else
      manifest->Set(path, std::move(lst));
  }
}

base::Value MergeManifest(const std::string& in_manifest) {
  // Following attributes will not be inherited from package.json 
  // Keep this list consistent with documents in `Manifest Format.md`
  static std::vector<const char*> non_inherited_attrs = {
                                                    switches::kmFullscreen,
                                                    switches::kmKiosk,
                                                    switches::kmPosition,
                                                    switches::kmResizable,
                                                    switches::kmShow
                                                    };
  base::Value::Dict manifest;

  // retrieve `window` manifest set by `new-win-policy`
  std::string manifest_str = in_manifest.empty() ? base::UTF16ToUTF8(nw::GetCurrentNewWinManifest())
    : in_manifest;
  std::unique_ptr<base::Value> val(base::JSONReader::ReadDeprecated(manifest_str));
  if (val && val->is_dict())
    manifest = val->GetDict().Clone();

  // merge with default `window` manifest in package.json if exists
  nw::Package* pkg = nw::package();
  if (pkg) {
    std::string js_doc_start, js_doc_end;
    std::string* str = pkg->root()->FindString(::switches::kmInjectJSDocStart);
    if (str)
      js_doc_start = *str;
    if (!js_doc_start.empty())
      manifest.Set(::switches::kmInjectJSDocStart, js_doc_start);
    str = pkg->root()->FindString(::switches::kmInjectJSDocEnd);
    if (str)
      js_doc_end = *str;
    if (!js_doc_end.empty())
      manifest.Set(::switches::kmInjectJSDocEnd, js_doc_end);
    base::Value::Dict* manifest_window = pkg->window();
    if (manifest_window) {
      base::Value::Dict manifest_window_cloned = manifest_window->Clone();
      // filter out non inherited attributes
      std::vector<const char*>::iterator it;
      for(it = non_inherited_attrs.begin(); it != non_inherited_attrs.end(); it++) {
        manifest_window_cloned.Remove(*it);
      }
      // overwrite default `window` manifest with the one passed by `new-win-policy`
      manifest_window_cloned.Merge(std::move(manifest));
      return base::Value(std::move(manifest_window_cloned));
    }
  }
  return base::Value(std::move(manifest));
}

} //namespace

void SetAppIcon(gfx::Image &app_icon);

#if defined(OS_WIN)
void SetAppHIcon(base::win::ScopedHICON icon);
void SetWindowHIcon(base::win::ScopedHICON icon);
#endif // OS_WIN

// browser

bool RphGuestFilterURLHook(RenderProcessHost* rph, const GURL* url)  {
  extensions::WebViewRendererState* renderer_state =
      extensions::WebViewRendererState::GetInstance();
  std::string owner_extension;
  int process_id = rph->GetID();
  if (!renderer_state->GetOwnerInfo(process_id, nullptr, &owner_extension))
    return false;
  const Extension* extension =
    ExtensionRegistry::Get(rph->GetBrowserContext())->enabled_extensions().GetByID(owner_extension);
  if (!extension)
    return false;
  bool file_scheme = false;
  if (WebviewInfo::IsURLWebviewAccessible(extension,
                                          WebViewGuest::GetPartitionID(rph),
                                          *url, &file_scheme)) {
    if (file_scheme) {
      content::ChildProcessSecurityPolicy::GetInstance()->GrantRequestScheme(
          process_id, url::kFileScheme);
    }
    return true;
  }
  return false;
}

typedef bool (*RphGuestFilterURLHookFn)(content::RenderProcessHost* rph, const GURL* url);
CONTENT_EXPORT extern RphGuestFilterURLHookFn gRphGuestFilterURLHook;

bool GuestSwapProcessHook(content::BrowserContext* browser_context, const GURL& url) {
  if (!url.SchemeIs("chrome-extension"))
    return false;
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser_context);
  std::string extension_id = url.host();
  const Extension* extension = registry->enabled_extensions().GetByID(extension_id);
  if (extension && !extensions::ManifestURL::Get(extension, "devtools_page").is_empty())
    return false;
  return false;
}

typedef bool(*GuestSwapProcessHookFn)(content::BrowserContext*, const GURL& url);
CONTENT_EXPORT extern GuestSwapProcessHookFn gGuestSwapProcessHook;

void LoadNWAppAsExtensionHook(base::Value::Dict* manifest,
                              const base::FilePath& extension_path,
                              std::string* error) {
  gRphGuestFilterURLHook = RphGuestFilterURLHook;
  gGuestSwapProcessHook = GuestSwapProcessHook;
  if (!manifest)
    return;

  std::string *main_url;
  base::Value *node_remote = NULL;

  const base::CommandLine* cmdline = base::CommandLine::ForCurrentProcess();
  nw::Package* package = cmdline->HasSwitch("nwjs-test-mode") ?
    nw::package(&extension_path) : nw::package();
  manifest->Set(manifest_keys::kNWJSInternalFlag, true);
  if (error && !package->cached_error_content().empty()) {
    *error = package->cached_error_content();
    return;
  }

  base::Value::Dict cloned_root;
  for (auto pair: *(package->root())) {
    cloned_root.Set(pair.first, pair.second.Clone());
  }
  manifest->Set(manifest_keys::kNWJSInternalManifest, std::move(cloned_root));

  if ((main_url = manifest->FindString(manifest_keys::kNWJSMain))) {
    if (base::EndsWith(*main_url, ".js", base::CompareCase::INSENSITIVE_ASCII)) {
      AmendManifestStringList(manifest, manifest_keys::kPlatformAppBackgroundScripts, *main_url);
      manifest->Set(manifest_keys::kNWJSInternalMainFilename, *main_url);
    }else if (base::FeatureList::IsEnabled(::features::kNWNewWin))
      AmendManifestStringList(manifest, manifest_keys::kPlatformAppBackgroundScripts, "nwjs/newwin.js");
    else
      AmendManifestStringList(manifest, manifest_keys::kPlatformAppBackgroundScripts, "nwjs/default.js");

    std::string* bg_script = manifest->FindString("bg-script");
    if (bg_script)
      AmendManifestStringList(manifest, manifest_keys::kPlatformAppBackgroundScripts, *bg_script);

    AmendManifestStringList(manifest, manifest_keys::kPermissions, "developerPrivate");
    AmendManifestStringList(manifest, manifest_keys::kPermissions, "management");
    AmendManifestStringList(manifest, manifest_keys::kPermissions, "<all_urls>");
  }

  std::string* icon_path = manifest->FindString("window.icon");
  if (icon_path) {
    gfx::Image app_icon;
    if (GetPackageImage(package, base::FilePath::FromUTF8Unsafe(*icon_path), &app_icon)) {
      if (app_icon.Width() > 128 || app_icon.Height() > 128) {
        const gfx::ImageSkia* originImageSkia = app_icon.ToImageSkia();
        gfx::ImageSkia resizedImageSkia =
          gfx::ImageSkiaOperations::CreateResizedImage(*originImageSkia,
                                                       skia::ImageOperations::RESIZE_GOOD,
                                                       gfx::Size(128, 128));
        app_icon = gfx::Image(resizedImageSkia);
      }
      SetAppIcon(app_icon);
      int width = app_icon.Width();
      std::string key = "icons." + base::NumberToString(width);
      manifest->Set(key, *icon_path);
#if defined(OS_WIN)
      SetWindowHIcon((IconUtil::CreateHICONFromSkBitmapSizedTo(*app_icon.AsImageSkia().bitmap(),
                      GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON))));
      SetAppHIcon((IconUtil::CreateHICONFromSkBitmapSizedTo(*app_icon.AsImageSkia().bitmap(),
                      GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON))));
#endif
    }
  }

  if ((node_remote = manifest->Find(switches::kmRemotePages))) {
    //FIXME: node-remote spec different with kWebURLs
    std::string node_remote_string;
    base::Value::List node_remote_list;
    if (node_remote->is_string()) {
      base::Value::List _node_remote_list;
      _node_remote_list.Append(node_remote->GetString());
      node_remote_list = std::move(_node_remote_list);
    } else
      node_remote_list = node_remote->GetList().Clone();

    if (node_remote_list.size())
      AmendManifestList(manifest, manifest_keys::kWebURLs, node_remote_list);
  }

  if (NWContentVerifierDelegate::GetDefaultMode() == NWContentVerifierDelegate::ENFORCE_STRICT)
    manifest->Set(manifest_keys::kNWJSContentVerifyFlag, true);

  if (package->temp_dir().IsValid()) {
    // need to remove the dir in renderer process
    manifest->Set("nw-temp-dir", package->temp_dir().GetPath().AsUTF8Unsafe());
  }
}

void CalcNewWinParams(content::WebContents* new_contents, void* params,
                      std::string* nw_inject_js_doc_start,
                      std::string* nw_inject_js_doc_end,
                      const std::string& in_manifest) {
  extensions::AppWindow::CreateParams ret;
  std::unique_ptr<base::Value> val;
  base::Value manifest_d = MergeManifest(in_manifest);
  base::Value::Dict& manifest = manifest_d.GetDict();
  absl::optional<bool> resizable = manifest.FindBool(switches::kmResizable);
  if (resizable) {
    ret.resizable = *resizable;
  }
  absl::optional<bool> fullscreen = manifest.FindBool(switches::kmFullscreen);
  if (fullscreen && *fullscreen) {
    ret.state = ui::SHOW_STATE_FULLSCREEN;
  }
  absl::optional<int> width = manifest.FindInt(switches::kmWidth);
  absl::optional<int> height = manifest.FindInt(switches::kmHeight);
  if (width)
    ret.content_spec.bounds.set_width(*width);
  if (height)
    ret.content_spec.bounds.set_height(*height);

  absl::optional<int> x = manifest.FindInt(switches::kmX);
  absl::optional<int> y = manifest.FindInt(switches::kmY);
  if (x)
    ret.window_spec.bounds.set_x(*x);
  if (y)
    ret.window_spec.bounds.set_y(*y);
  absl::optional<bool> top = manifest.FindBool(switches::kmAlwaysOnTop);
  if (top && *top) {
    ret.always_on_top = true;
  }
  absl::optional<bool> frame = manifest.FindBool(switches::kmFrame);
  if (frame && !(*frame)) {
    ret.frame = extensions::AppWindow::FRAME_NONE;
  }
  absl::optional<bool> all_workspaces =
    manifest.FindBool(switches::kmVisibleOnAllWorkspaces);
  if (all_workspaces && *all_workspaces) {
    ret.visible_on_all_workspaces = true;
  }
  gfx::Size& minimum_size = ret.content_spec.minimum_size;
  absl::optional<int> min_height = manifest.FindInt(switches::kmMinHeight);
  absl::optional<int> min_width = manifest.FindInt(switches::kmMinWidth);
  if (min_width)
    minimum_size.set_width(*min_width);
  if (min_height)
    minimum_size.set_height(*min_height);
  absl::optional<int> max_height = manifest.FindInt(switches::kmMaxHeight);
  absl::optional<int> max_width = manifest.FindInt(switches::kmMaxWidth);
  gfx::Size& maximum_size = ret.content_spec.maximum_size;
  if (max_width)
    maximum_size.set_width(*max_width);
  if (max_height)
    maximum_size.set_height(*max_height);

  *(extensions::AppWindow::CreateParams*)params = ret;

  std::string* str = manifest.FindString(switches::kmInjectJSDocStart);
  if (str)
    *nw_inject_js_doc_start = *str;
  str = manifest.FindString(switches::kmInjectJSDocEnd);
  if (str)
    *nw_inject_js_doc_end = *str;
}

bool ExecuteAppCommandHook(int command_id, extensions::AppWindow* app_window) {
#if defined(OS_MAC)
  return false;
#else
  //nw::ObjectManager* obj_manager = nw::ObjectManager::Get(app_window->browser_context());
  //Menu* menu = (Menu*)obj_manager->GetApiObject(command_id);
  Menu* menu = app_window->menu_;
  if (!menu)
    return false;
  menu->menu_delegate_->ExecuteCommand(command_id, 0);
  return true;
#endif //OSX
}

} // namespace nw
