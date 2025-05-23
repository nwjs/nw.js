#include "content/nw/src/api/nw_clipboard_api.h"

#include "base/base64.h"
#include "base/logging.h"
#include "base/run_loop.h"
#include "base/test/bind.h"
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
#include "ui/base/clipboard/clipboard_constants.h"
#include "ui/base/clipboard/scoped_clipboard_writer.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "third_party/skia/include/core/SkBitmap.h"

#pragma clang diagnostic ignored "-Wunreachable-code-break"

using namespace extensions::nwapi::nw__clipboard;

namespace extensions {

namespace {
  const char* kPNGDataUriPrefix = "data:image/png;base64,";
  const char* kJPEGDataUriPrefix = "data:image/jpeg;base64,";
  const int   kQuality = 100;

class ReadImageHelper {
public:
  ReadImageHelper() = default;
  ~ReadImageHelper() = default;

  std::vector<uint8_t> ReadPng(ui::Clipboard* clipboard) {
    base::RunLoop loop;
    std::vector<uint8_t> png;
    clipboard->ReadPng(
                       ui::ClipboardBuffer::kCopyPaste,
                       /* data_dst = */ nullptr,
                       base::BindLambdaForTesting([&](const std::vector<uint8_t>& result) {
                                                    png = result;
                                                    loop.Quit();
                                                  }));
    loop.Run();
    return png;
  }
};

std::vector<uint8_t> ReadPng(ui::Clipboard* clipboard) {
  ReadImageHelper read_image_helper;
  return read_image_helper.ReadPng(clipboard);
}

  class ClipboardReader {
  public:
    ClipboardReader() {
      clipboard_ = ui::Clipboard::GetForCurrentThread();
    }

    bool Read(ClipboardData& data) {
      switch(data.type) {
        case Type::kText:
        return ReadText(data);
        break;
        case Type::kHtml:
        return ReadHTML(data);
        break;
        case Type::kRtf:
        return ReadRTF(data);
        break;
        case Type::kPng:
        case Type::kJpeg:
        return ReadImage(data);
        break;
        case Type::kNone:
        return false;
      }
      return false;
    }

    std::string error() {
      return error_;
    }

  private:
    bool ReadText(ClipboardData& data) {
      DCHECK(data.type == Type::kText);
      std::u16string text;
      clipboard_->ReadText(ui::ClipboardBuffer::kCopyPaste, nullptr, &text);
      data.data.emplace(std::string(base::UTF16ToUTF8(text)));
      return true;
    }

    bool ReadHTML(ClipboardData& data) {
      DCHECK(data.type == Type::kHtml);
      std::u16string text;
      std::string src_url;
      uint32_t fragment_start, fragment_end;
      clipboard_->ReadHTML(ui::ClipboardBuffer::kCopyPaste, nullptr,
                           &text, &src_url, &fragment_start, &fragment_end);
      data.data.emplace(std::string(base::UTF16ToUTF8(text)));
      return true;
    }

    bool ReadRTF(ClipboardData& data) {
      DCHECK(data.type == Type::kRtf);
      std::string text;
      clipboard_->ReadRTF(ui::ClipboardBuffer::kCopyPaste, nullptr, &text);
      data.data.emplace(std::string(text));
      return true;
    }

    bool ReadImage(ClipboardData& data) {
      DCHECK(data.type == Type::kPng || data.type == Type::kJpeg);
      std::optional<std::vector<uint8_t>> encoded_image;
      std::vector<uint8_t> png = ReadPng(clipboard_);

      if (data.type == Type::kPng) {
        encoded_image = std::move(png);
      } else if (data.type == Type::kJpeg) {
        SkBitmap bitmap;
        bitmap = gfx::PNGCodec::Decode(png);
	encoded_image = gfx::JPEGCodec::Encode(bitmap, kQuality);
        if (!encoded_image) {
          LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(data.type) << ") failed when converting to JPEG";
          error_ = "Failed to encode as JPEG";
          return false;
        }
      }

      std::string encoded_image_base64;
      std::string encoded_image_str(base::as_string_view(encoded_image.value()));
      encoded_image_base64 = base::Base64Encode(encoded_image_str);

      if (!(data.raw && *(data.raw))) {
        if (data.type == Type::kPng) {
          encoded_image_base64.insert(0, kPNGDataUriPrefix);
        } else {
          DCHECK(data.type == Type::kJpeg);
          encoded_image_base64.insert(0, kJPEGDataUriPrefix);
        }
      }

      data.data.emplace(std::string(encoded_image_base64));
      LOG(INFO) << "NwClipboardGetSyncFunction::RunSync(" << nwapi::nw__clipboard::ToString(data.type) << ")";
      return true;
    }

    raw_ptr<ui::Clipboard> clipboard_;
    std::string error_;
  };

  class ClipboardWriter {
  public:
    ClipboardWriter() {
      scw_.reset(new ui::ScopedClipboardWriter(ui::ClipboardBuffer::kCopyPaste));
    }

    ~ClipboardWriter() {
      scw_.reset();
    }

    bool Write(ClipboardData& data) {
      switch(data.type) {
        case Type::kText:
        return WriteText(data);
        break;
        case Type::kHtml:
        return WriteHTML(data);
        break;
        case Type::kRtf:
        return WriteRTF(data);
        break;
        case Type::kPng:
        case Type::kJpeg:
        return WriteImage(data);
        break;
        case Type::kNone:
        return false;
      }
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
      DCHECK(data.type == Type::kText);
      scw_->WriteText(base::UTF8ToUTF16(*(data.data)));
      return true;
    }

    bool WriteHTML(ClipboardData& data) {
      DCHECK(data.type == Type::kHtml);
      scw_->WriteHTML(base::UTF8ToUTF16(*(data.data)), std::string());
      return true;
    }

    bool WriteRTF(ClipboardData& data) {
      DCHECK(data.type == Type::kRtf);
      scw_->WriteRTF(*(data.data));
      return true;
    }

    bool WriteImage(ClipboardData& data) {
      DCHECK(data.type == Type::kPng || data.type == Type::kJpeg);
      std::string content = *(data.data);

      // strip off data uri header if raw is set
      if (!(data.raw && *(data.raw))) {
        if (data.type == Type::kPng && base::StartsWith(content, kPNGDataUriPrefix, base::CompareCase::INSENSITIVE_ASCII)) {
          content = content.substr(strlen(kPNGDataUriPrefix));
        } else if (data.type == Type::kJpeg && base::StartsWith(content, kJPEGDataUriPrefix, base::CompareCase::INSENSITIVE_ASCII)) {
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

      SkBitmap bitmap;
      if (data.type == Type::kPng) {
        bitmap = gfx::PNGCodec::Decode(base::as_byte_span(decoded_str));
	if (bitmap.isNull()) {
	  error_ = "Failed to decode as PNG";
	  return false;
	}
      } else if (data.type == Type::kJpeg) {
        bitmap = gfx::JPEGCodec::Decode(base::as_byte_span(decoded_str));
        if (bitmap.isNull()) {
          error_ = "Failed to decode as JPEG";
          return false;
        }
      }

      scw_->WriteImage(bitmap);

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

bool NwClipboardGetListSyncFunction::RunNWSync(base::Value::List* response, std::string* error) {
  std::optional<GetListSync::Params> params(GetListSync::Params::Create(args()));
  EXTENSION_FUNCTION_VALIDATE(params.has_value());
  std::unique_ptr<ClipboardReader> reader(new ClipboardReader());

  for(auto& data : params->data) {
    if (!reader->Read(data)) {
      *error = reader->error();
      return false;
    }
    response->Append(base::Value(data.ToValue()));
  }

  return true;
}

NwClipboardSetListSyncFunction::NwClipboardSetListSyncFunction() {
}

NwClipboardSetListSyncFunction::~NwClipboardSetListSyncFunction() {
}

bool NwClipboardSetListSyncFunction::RunNWSync(base::Value::List* response, std::string* error) {
  std::optional<SetListSync::Params> params(SetListSync::Params::Create(args()));
  EXTENSION_FUNCTION_VALIDATE(params.has_value());
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

bool NwClipboardClearSyncFunction::RunNWSync(base::Value::List* response, std::string* error) {
  ui::Clipboard* clipboard = ui::Clipboard::GetForCurrentThread();
  clipboard->Clear(ui::ClipboardBuffer::kCopyPaste);
  return true;
}

NwClipboardReadAvailableTypesFunction::NwClipboardReadAvailableTypesFunction() {

}

NwClipboardReadAvailableTypesFunction::~NwClipboardReadAvailableTypesFunction() {

}

bool NwClipboardReadAvailableTypesFunction::RunNWSync(base::Value::List* response, std::string* error) {
  ui::Clipboard* clipboard = ui::Clipboard::GetForCurrentThread();
  std::vector<std::u16string> types;
  clipboard->ReadAvailableTypes(ui::ClipboardBuffer::kCopyPaste, nullptr, &types);
  for(std::vector<std::u16string>::iterator it = types.begin(); it != types.end(); it++) {
    if (base::EqualsASCII(*it, ui::kMimeTypePlainText)) {
      response->Append(ToString(Type::kText));
    } else if (base::EqualsASCII(*it, ui::kMimeTypeHtml)) {
      response->Append(ToString(Type::kHtml));
    } else if (base::EqualsASCII(*it, ui::kMimeTypeRtf)) {
      response->Append(ToString(Type::kRtf));
    } else if (base::EqualsASCII(*it, ui::kMimeTypePng)) {
      response->Append(ToString(Type::kPng));
      response->Append(ToString(Type::kJpeg));
    }
  }
  return true;
}

} // namespace extensions
