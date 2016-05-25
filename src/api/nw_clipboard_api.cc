#include "content/nw/src/api/nw_clipboard_api.h"

#include "base/base64.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "content/nw/src/api/nw_clipboard.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"
#include "ui/base/clipboard/clipboard.h"
#include "ui/base/clipboard/scoped_clipboard_writer.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "third_party/skia/include/core/SkBitmap.h"

using namespace extensions::nwapi::nw__clipboard;

namespace extensions {

namespace {
  const char* kPNGDataUriPrefix = "data:image/png;base64,";
  const char* kJPEGDataUriPrefix = "data:image/jpeg;base64,";
  const int   kQulity = 100;
}

NwClipboardGetSyncFunction::NwClipboardGetSyncFunction() {
}

NwClipboardGetSyncFunction::~NwClipboardGetSyncFunction() {
}

bool NwClipboardGetSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  std::unique_ptr<GetSync::Params> params(GetSync::Params::Create(*args_));
  ui::Clipboard* clipboard = ui::Clipboard::GetForCurrentThread();

  if (params->type == TYPE_TEXT) {
    base::string16 text;
    clipboard->ReadText(ui::CLIPBOARD_TYPE_COPY_PASTE, &text);
    response->Append(new base::StringValue(text));
    LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(text)";
    return true;
  } else if (params->type == TYPE_PNG || params->type == TYPE_JPEG) {
    std::vector<unsigned char> encoded_image;
    SkBitmap bitmap = clipboard->ReadImage(ui::CLIPBOARD_TYPE_COPY_PASTE);
    SkAutoLockPixels lock(bitmap);

    if (bitmap.isNull()) {
      response->Append(new base::StringValue(""));
      return true;
    }

    if (params->type == TYPE_PNG &&
       !gfx::PNGCodec::EncodeA8SkBitmap(bitmap, &encoded_image)) {
      LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(params->type) << ") failed when converting to PNG";
      *error = "Failed to encode as PNG";
      return false;
    } else if (params->type == TYPE_JPEG &&
               !gfx::JPEGCodec::Encode(reinterpret_cast<const unsigned char*>(bitmap.getPixels()),
                                      gfx::JPEGCodec::FORMAT_SkBitmap,
                                      bitmap.width(),
                                      bitmap.height(),
                                      bitmap.rowBytes(),
                                      kQulity,
                                      &encoded_image)) {
      LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(params->type) << ") failed when converting to JPEG";
      *error = "Failed to encode as JPEG";
      return false;
    }

    std::string encoded_image_base64;
    std::string encoded_image_str(encoded_image.data(), encoded_image.data() + encoded_image.size());
    base::Base64Encode(encoded_image_str, &encoded_image_base64);

    if (!params->raw) {
      if (params->type == TYPE_PNG) {
        encoded_image_base64.insert(0, kPNGDataUriPrefix);
      } else if (params->type == TYPE_JPEG) {
        encoded_image_base64.insert(0, kJPEGDataUriPrefix);
      }
    }

    response->Append(new base::StringValue(encoded_image_base64));
    LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(params->type) << ")";
    return true;
  } else if (params->type == TYPE_HTML) {
    base::string16 text;
    std::string src_url;
    uint32_t fragment_start, fragment_end;
    clipboard->ReadHTML(ui::CLIPBOARD_TYPE_COPY_PASTE, &text, &src_url, &fragment_start, &fragment_end);
    response->Append(new base::StringValue(text));
    LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(html)";
    return true;
  } else if (params->type == TYPE_RTF) {
    std::string text;
    clipboard->ReadRTF(ui::CLIPBOARD_TYPE_COPY_PASTE, &text);
    response->Append(new base::StringValue(text));
    LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(rtf)";
    return true;
  } else {
    NOTREACHED();
    return false;
  }
 
}

NwClipboardSetSyncFunction::NwClipboardSetSyncFunction() {

}

NwClipboardSetSyncFunction::~NwClipboardSetSyncFunction() {
}


bool NwClipboardSetSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  std::unique_ptr<SetSync::Params> params(SetSync::Params::Create(*args_));
  ui::ScopedClipboardWriter scw(ui::CLIPBOARD_TYPE_COPY_PASTE);

  if (params->type == TYPE_TEXT) {
    scw.WriteText(base::UTF8ToUTF16(params->content));
  } else if (params->type == TYPE_PNG || params->type == TYPE_JPEG) {

    std::string content = params->content;

    // strip off data uri header if raw is set
    if (!params->raw) {
      if (params->type == TYPE_PNG && base::StartsWith(content, kPNGDataUriPrefix, base::CompareCase::INSENSITIVE_ASCII)) {
          content = content.substr(strlen(kPNGDataUriPrefix));
      } else if (params->type == TYPE_JPEG && base::StartsWith(content, kJPEGDataUriPrefix, base::CompareCase::INSENSITIVE_ASCII)) {
          content = content.substr(strlen(kJPEGDataUriPrefix));
      } else {
        *error = base::StringPrintf("Invalid data URI. Only \"%s\" or \"%s\" is accepted.", kPNGDataUriPrefix, kJPEGDataUriPrefix);
        return false;
      }
    }

    std::string decoded_str;
    if (!base::Base64Decode(content, &decoded_str)) {
      *error = "Bad base64 string";
      return false;
    }

    std::unique_ptr<SkBitmap> bitmap(new SkBitmap());
    if (params->type == TYPE_PNG &&
      !gfx::PNGCodec::Decode(reinterpret_cast<const unsigned char*>(decoded_str.c_str()), decoded_str.size(), bitmap.get())) {
      *error = "Failed to decode as PNG";
      return false;
    } else if (params->type == TYPE_JPEG) {
      bitmap = gfx::JPEGCodec::Decode(reinterpret_cast<const unsigned char*>(decoded_str.c_str()), decoded_str.size());
      if (!bitmap) {
        *error = "Failed to decode as JPEG";
        return false;
      }
    }

    scw.WriteImage(*bitmap);
  } else if (params->type == TYPE_HTML) {
    scw.WriteHTML(base::UTF8ToUTF16(params->content), std::string());
  } else if (params->type == TYPE_RTF) {
    scw.WriteRTF(params->content);
  } else {
    NOTREACHED();
    return false;
  }

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

NwClipboardReadAvailableTypesFunction::NwClipboardReadAvailableTypesFunction() {

}

NwClipboardReadAvailableTypesFunction::~NwClipboardReadAvailableTypesFunction() {

}

bool NwClipboardReadAvailableTypesFunction::RunNWSync(base::ListValue* response, std::string* error) {
  ui::Clipboard* clipboard = ui::Clipboard::GetForCurrentThread();
  bool contains_filenames;
  std::vector<base::string16> types;
  clipboard->ReadAvailableTypes(ui::CLIPBOARD_TYPE_COPY_PASTE, &types, &contains_filenames);
  for(std::vector<base::string16>::iterator it = types.begin(); it != types.end(); it++) {
    if (base::EqualsASCII(*it, ui::Clipboard::kMimeTypeText)) {
      response->Append(new base::StringValue(ToString(TYPE_TEXT)));
    } else if (base::EqualsASCII(*it, ui::Clipboard::kMimeTypeHTML)) {
      response->Append(new base::StringValue(ToString(TYPE_HTML)));
    } else if (base::EqualsASCII(*it, ui::Clipboard::kMimeTypeRTF)) {
      response->Append(new base::StringValue(ToString(TYPE_RTF)));
    } else if (base::EqualsASCII(*it, ui::Clipboard::kMimeTypePNG)) {
      response->Append(new base::StringValue(ToString(TYPE_PNG)));
      response->Append(new base::StringValue(ToString(TYPE_JPEG)));
    }
  }
  return true;
}

} // namespace extensions
