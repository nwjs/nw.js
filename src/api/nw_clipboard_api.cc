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

  class ClipboardReader {
  public:
    ClipboardReader() {
      clipboard_ = ui::Clipboard::GetForCurrentThread();
    }

    bool Read(ClipboardData& data) {
      switch(data.type) {
        case TYPE_TEXT:
        return ReadText(data);
        break;
        case TYPE_HTML:
        return ReadHTML(data);
        break;
        case TYPE_RTF:
        return ReadRTF(data);
        break;
        case TYPE_PNG:
        case TYPE_JPEG:
        return ReadImage(data);
        break;
        case TYPE_NONE:
        NOTREACHED();
        return false;
      }
      NOTREACHED();
      return false;      
    }

    std::string error() {
      return error_;
    }

  private:
    bool ReadText(ClipboardData& data) {
      DCHECK(data.type == TYPE_TEXT);
      base::string16 text;
      clipboard_->ReadText(ui::CLIPBOARD_TYPE_COPY_PASTE, &text);
      data.data.reset(new std::string(base::UTF16ToUTF8(text)));
      return true;
    }

    bool ReadHTML(ClipboardData& data) {
      DCHECK(data.type == TYPE_HTML);
      base::string16 text;
      std::string src_url;
      uint32_t fragment_start, fragment_end;
      clipboard_->ReadHTML(ui::CLIPBOARD_TYPE_COPY_PASTE, &text, &src_url, &fragment_start, &fragment_end);
      data.data.reset(new std::string(base::UTF16ToUTF8(text)));
      return true;
    }

    bool ReadRTF(ClipboardData& data) {
      DCHECK(data.type == TYPE_RTF);
      std::string text;
      clipboard_->ReadRTF(ui::CLIPBOARD_TYPE_COPY_PASTE, &text);
      data.data.reset(new std::string(text));
      return true;
    }

    bool ReadImage(ClipboardData& data) {
      DCHECK(data.type == TYPE_PNG || data.type == TYPE_JPEG);
      std::vector<unsigned char> encoded_image;
      SkBitmap bitmap = clipboard_->ReadImage(ui::CLIPBOARD_TYPE_COPY_PASTE);
      SkAutoLockPixels lock(bitmap);

      if (bitmap.isNull()) {
        return true;
      }

      if (data.type == TYPE_PNG &&
         !gfx::PNGCodec::EncodeA8SkBitmap(bitmap, &encoded_image)) {
        LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(data.type) << ") failed when converting to PNG";
        error_ = "Failed to encode as PNG";
        return false;
      } else if (data.type == TYPE_JPEG &&
                 !gfx::JPEGCodec::Encode(reinterpret_cast<const unsigned char*>(bitmap.getPixels()),
                                        gfx::JPEGCodec::FORMAT_SkBitmap,
                                        bitmap.width(),
                                        bitmap.height(),
                                        bitmap.rowBytes(),
                                        kQulity,
                                        &encoded_image)) {
        LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(data.type) << ") failed when converting to JPEG";
        error_ = "Failed to encode as JPEG";
        return false;
      }

      std::string encoded_image_base64;
      std::string encoded_image_str(encoded_image.data(), encoded_image.data() + encoded_image.size());
      base::Base64Encode(encoded_image_str, &encoded_image_base64);

      if (!(data.raw.get() && *(data.raw))) {
        if (data.type == TYPE_PNG) {
          encoded_image_base64.insert(0, kPNGDataUriPrefix);
        } else {
          DCHECK(data.type == TYPE_JPEG);
          encoded_image_base64.insert(0, kJPEGDataUriPrefix);
        }
      }

      data.data.reset(new std::string(encoded_image_base64));
      LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(data.type) << ")";
      return true;
    }

    ui::Clipboard* clipboard_;
    std::string error_;
  };

  class ClipboardWriter {
  public:
    ClipboardWriter() {
      scw_.reset(new ui::ScopedClipboardWriter(ui::CLIPBOARD_TYPE_COPY_PASTE));
    }

    ~ClipboardWriter() {
      scw_.reset();
    }

    bool Write(ClipboardData& data) {
      switch(data.type) {
        case TYPE_TEXT:
        return WriteText(data);
        break;
        case TYPE_HTML:
        return WriteHTML(data);
        break;
        case TYPE_RTF:
        return WriteRTF(data);
        break;
        case TYPE_PNG:
        case TYPE_JPEG:
        return WriteImage(data);
        break;
        case TYPE_NONE:
        NOTREACHED();
        return false;
      }
      NOTREACHED();
      return false;
    }

    std::string error() const {
      return error_;
    }

    void Reset() {
      scw_->Reset();
    }

  private:
    bool WriteText(ClipboardData& data) {
      DCHECK(data.type == TYPE_TEXT);
      scw_->WriteText(base::UTF8ToUTF16(*(data.data)));
      return true;
    }

    bool WriteHTML(ClipboardData& data) {
      DCHECK(data.type == TYPE_HTML);
      scw_->WriteHTML(base::UTF8ToUTF16(*(data.data)), std::string());
      return true;
    }

    bool WriteRTF(ClipboardData& data) {
      DCHECK(data.type == TYPE_RTF);
      scw_->WriteRTF(*(data.data));
      return true;
    }

    bool WriteImage(ClipboardData& data) {
      DCHECK(data.type == TYPE_PNG || data.type == TYPE_JPEG);
      std::string content = *(data.data);

      // strip off data uri header if raw is set
      if (!(data.raw.get() && *(data.raw))) {
        if (data.type == TYPE_PNG && base::StartsWith(content, kPNGDataUriPrefix, base::CompareCase::INSENSITIVE_ASCII)) {
          content = content.substr(strlen(kPNGDataUriPrefix));
        } else if (data.type == TYPE_JPEG && base::StartsWith(content, kJPEGDataUriPrefix, base::CompareCase::INSENSITIVE_ASCII)) {
          content = content.substr(strlen(kJPEGDataUriPrefix));
        } else {
          error_ = base::StringPrintf("Invalid data URI. Only \"%s\" or \"%s\" is accepted.", kPNGDataUriPrefix, kJPEGDataUriPrefix);
          return false;
        }
      }

      std::string decoded_str;
      if (!base::Base64Decode(content, &decoded_str)) {
        error_ = "Bad base64 string";
        return false;
      }

      std::unique_ptr<SkBitmap> bitmap(new SkBitmap());
      if (data.type == TYPE_PNG &&
        !gfx::PNGCodec::Decode(reinterpret_cast<const unsigned char*>(decoded_str.c_str()), decoded_str.size(), bitmap.get())) {
        error_ = "Failed to decode as PNG";
        return false;
      } else if (data.type == TYPE_JPEG) {
        std::unique_ptr<SkBitmap> tmp_bitmap = gfx::JPEGCodec::Decode(reinterpret_cast<const unsigned char*>(decoded_str.c_str()), decoded_str.size());
        if (tmp_bitmap == NULL) {
          error_ = "Failed to decode as JPEG";
          return false;
        }
        bitmap = std::move(tmp_bitmap);
      }

      scw_->WriteImage(*bitmap);

      return true;
    }

    std::string error_;
    std::unique_ptr<ui::ScopedClipboardWriter> scw_;
  };
}

NwClipboardGetListSyncFunction::NwClipboardGetListSyncFunction() {
}

NwClipboardGetListSyncFunction::~NwClipboardGetListSyncFunction() {
}

bool NwClipboardGetListSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  std::unique_ptr<GetListSync::Params> params(GetListSync::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());
  std::unique_ptr<ClipboardReader> reader(new ClipboardReader());

  for(auto& data : params->data) {
    if (!reader->Read(data)) {
      *error = reader->error();
      return false;
    }
    response->Append(data.ToValue());
  }

  return true;
}

NwClipboardSetListSyncFunction::NwClipboardSetListSyncFunction() {
}

NwClipboardSetListSyncFunction::~NwClipboardSetListSyncFunction() {
}

bool NwClipboardSetListSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
  std::unique_ptr<SetListSync::Params> params(SetListSync::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());
  std::unique_ptr<ClipboardWriter> writer(new ClipboardWriter());

  for(auto& data : params->data) {
    if (!writer->Write(data)) {
      *error = writer->error();
      writer->Reset();
      return false;
    }
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
      response->Append(base::WrapUnique(new base::Value(ToString(TYPE_TEXT))));
    } else if (base::EqualsASCII(*it, ui::Clipboard::kMimeTypeHTML)) {
      response->Append(base::WrapUnique(new base::Value(ToString(TYPE_HTML))));
    } else if (base::EqualsASCII(*it, ui::Clipboard::kMimeTypeRTF)) {
      response->Append(base::WrapUnique(new base::Value(ToString(TYPE_RTF))));
    } else if (base::EqualsASCII(*it, ui::Clipboard::kMimeTypePNG)) {
      response->Append(base::WrapUnique(new base::Value(ToString(TYPE_PNG))));
      response->Append(base::WrapUnique(new base::Value(ToString(TYPE_JPEG))));
    }
  }
  return true;
}

} // namespace extensions
