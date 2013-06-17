#include "content/nw/src/api/ti_bindings.h"

#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/values.h"
#include "base/command_line.h"
#include "chrome/renderer/static_v8_external_string_resource.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/bindings_common.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/v8_value_converter.h"
#include "grit/nw_resources.h"

using content::RenderView;
using content::V8ValueConverter;
using base::FilePath;

namespace api {
  
  TiAppBindings::TiAppBindings()
  : v8::Extension("Application.js",
                  GetStringResource(
                                    IDR_TI_API_APPLICATION_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_APPLICATION_JS).size()) {
                  }
  
  TiAppBindings::~TiAppBindings() {
  }
  
  
  
  TiBufferedStreamBindings::TiBufferedStreamBindings()
  : v8::Extension("BufferedStream.js",
                  GetStringResource(
                                    IDR_TI_API_BUFFERED_STREAM_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_BUFFERED_STREAM_JS).size()) {
                  }
  
  TiBufferedStreamBindings::~TiBufferedStreamBindings() {
  }
  
  
  
  TiClipboardBindings::TiClipboardBindings()
  : v8::Extension("Clipboard.js",
                  GetStringResource(
                                    IDR_TI_API_CLIPBOARD_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_CLIPBOARD_JS).size()) {
                  }
  
  TiClipboardBindings::~TiClipboardBindings() {
  }
  
  
  
  TiCompressionBindings::TiCompressionBindings()
  : v8::Extension("Compression.js",
                  GetStringResource(
                                    IDR_TI_API_COMPRESSION_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_COMPRESSION_JS).size()) {
                  }
  
  TiCompressionBindings::~TiCompressionBindings() {
  }
  
  
  
  TiDomainBindings::TiDomainBindings()
  : v8::Extension("Domain.js",
                  GetStringResource(
                                    IDR_TI_API_DOMAIN_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_DOMAIN_JS).size()) {
                  }
  
  TiDomainBindings::~TiDomainBindings() {
  }
  
  
  
  TiFileBindings::TiFileBindings()
  : v8::Extension("File.js",
                  GetStringResource(
                                    IDR_TI_API_FILE_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_FILE_JS).size()) {
                  }
  
  TiFileBindings::~TiFileBindings() {
  }
  
  
  
  TiFileSystemBindings::TiFileSystemBindings()
  : v8::Extension("FileSystem.js",
                  GetStringResource(
                                    IDR_TI_API_FILE_SYSTEM_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_FILE_SYSTEM_JS).size()) {
                  }
  
  TiFileSystemBindings::~TiFileSystemBindings() {
  }
  
  
  
  TiHashBindings::TiHashBindings()
  : v8::Extension("Hash.js",
                  GetStringResource(
                                    IDR_TI_API_HASH_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_HASH_JS).size()) {
                  }
  
  TiHashBindings::~TiHashBindings() {
  }
  
  
  
  TiMenuBindings::TiMenuBindings()
  : v8::Extension("Menu.js",
                  GetStringResource(
                                    IDR_TI_API_MENU_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_MENU_JS).size()) {
                  }
  
  TiMenuBindings::~TiMenuBindings() {
  }
  
  
  
  TiMenuItemBindings::TiMenuItemBindings()
  : v8::Extension("MenuItem.js",
                  GetStringResource(
                                    IDR_TI_API_MENU_ITEM_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_MENU_ITEM_JS).size()) {
                  }
  
  TiMenuItemBindings::~TiMenuItemBindings() {
  }
  
  
  
  TiPlatformBindings::TiPlatformBindings()
  : v8::Extension("Platform.js",
                  GetStringResource(
                                    IDR_TI_API_PLATFORM_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_PLATFORM_JS).size()) {
                  }
  
  TiPlatformBindings::~TiPlatformBindings() {
  }
  
  
  
  TiPropertyObjectBindings::TiPropertyObjectBindings()
  : v8::Extension("PropertyObject.js",
                  GetStringResource(
                                    IDR_TI_API_PROPERTY_OBJECT_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_PROPERTY_OBJECT_JS).size()) {
                  }
  
  TiPropertyObjectBindings::~TiPropertyObjectBindings() {
  }
  
  
  
  TiSocketBindings::TiSocketBindings()
  : v8::Extension("Socket.js",
                  GetStringResource(
                                    IDR_TI_API_SOCKET_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_SOCKET_JS).size()) {
                  }
  
  TiSocketBindings::~TiSocketBindings() {
  }
  
  
  
  
  TiWindowBindings::TiWindowBindings()
  : v8::Extension("TIWindow.js",
                  GetStringResource(
                                    IDR_TI_API_WINDOW_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_WINDOW_JS).size()) {
                  }
  
  TiWindowBindings::~TiWindowBindings() {
  }
  
  
  
  TiTrayBindings::TiTrayBindings()
  : v8::Extension("Tray.js",
                  GetStringResource(
                                    IDR_TI_API_TRAY_JS).data(),
                  0,     // num dependencies.
                  NULL,  // dependencies array.
                  GetStringResource(
                                    IDR_TI_API_TRAY_JS).size()) {
                  }
  
  TiTrayBindings::~TiTrayBindings() {
  }
}  // namespace api
