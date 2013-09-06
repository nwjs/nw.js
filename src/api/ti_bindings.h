#ifndef CONTENT_TI_SRC_API_BINDINGS_H_
#define CONTENT_TI_SRC_API_BINDINGS_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "v8/include/v8.h"
#include "grit/nw_resources.h"

namespace api {
	class TiBindings {
	public:
		static const int file_count;
		static const int files[];
	};
	const int TiBindings::file_count = 17;
	const int TiBindings::files[] = {
		IDR_TI_API_APPLICATION_JS,
		IDR_TI_API_BUFFERED_STREAM_JS,
		IDR_TI_API_CLIPBOARD_JS,
		IDR_TI_API_COMPRESSION_JS,
		IDR_TI_API_DOMAIN_JS,
		IDR_TI_API_FILE_JS,
		IDR_TI_API_FILE_SYSTEM_JS,
		IDR_TI_API_HASH_JS,
		IDR_TI_API_MENU_JS,
		IDR_TI_API_MENU_ITEM_JS,
		IDR_TI_API_PLATFORM_JS,
		IDR_TI_API_PROPERTY_OBJECT_JS,
		IDR_TI_API_SOCKET_JS,
		IDR_TI_API_CONNECTION_JS,
		IDR_TI_API_WINDOW_JS,
		IDR_TI_API_TRAY_JS,
		IDR_TI_API_FILEDIALOG_JS,
		IDR_TI_API_NOTIFY_JS
	};
}  // namespace api

#endif  // CONTENT_TI_SRC_API_BINDINGS_H_
