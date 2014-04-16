// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/common/gpu_internals.h"

#include "base/callback.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/string_number_conversions.h"
#include "base/stringprintf.h"
#include "base/sys_info.h"
#include "base/values.h"
#include "cc/switches.h"
#include "content/public/browser/compositor_util.h"
#include "content/public/browser/gpu_data_manager.h"
#include "content/public/browser/gpu_data_manager_observer.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/gpu_info.h"
#include "third_party/angle/src/common/version.h"

using content::GpuDataManager;
using content::GpuFeatureType;

namespace {

struct GpuFeatureInfo {
  std::string name;
  uint32 blocked;
  bool disabled;
  std::string disabled_description;
  bool fallback_to_software;
};

DictionaryValue* NewDescriptionValuePair(const std::string& desc,
    const std::string& value) {
  DictionaryValue* dict = new DictionaryValue();
  dict->SetString("description", desc);
  dict->SetString("value", value);
  return dict;
}

DictionaryValue* NewDescriptionValuePair(const std::string& desc,
    Value* value) {
  DictionaryValue* dict = new DictionaryValue();
  dict->SetString("description", desc);
  dict->Set("value", value);
  return dict;
}

Value* NewStatusValue(const char* name, const char* status) {
  DictionaryValue* value = new DictionaryValue();
  value->SetString("name", name);
  value->SetString("status", status);
  return value;
}

#if defined(OS_WIN)
// Output DxDiagNode tree as nested array of {description,value} pairs
ListValue* DxDiagNodeToList(const content::DxDiagNode& node) {
  ListValue* list = new ListValue();
  for (std::map<std::string, std::string>::const_iterator it =
      node.values.begin();
      it != node.values.end();
      ++it) {
    list->Append(NewDescriptionValuePair(it->first, it->second));
  }

  for (std::map<std::string, content::DxDiagNode>::const_iterator it =
      node.children.begin();
      it != node.children.end();
      ++it) {
    ListValue* sublist = DxDiagNodeToList(it->second);
    list->Append(NewDescriptionValuePair(it->first, sublist));
  }
  return list;
}
#endif

std::string GPUDeviceToString(const content::GPUInfo::GPUDevice& gpu) {
  std::string vendor = base::StringPrintf("0x%04x", gpu.vendor_id);
  if (!gpu.vendor_string.empty())
    vendor += " [" + gpu.vendor_string + "]";
  std::string device = base::StringPrintf("0x%04x", gpu.device_id);
  if (!gpu.device_string.empty())
    device += " [" + gpu.device_string + "]";
  return base::StringPrintf(
      "VENDOR = %s, DEVICE= %s", vendor.c_str(), device.c_str());
}

DictionaryValue* GpuInfoAsDictionaryValue() {
  content::GPUInfo gpu_info = GpuDataManager::GetInstance()->GetGPUInfo();
  ListValue* basic_info = new ListValue();
  basic_info->Append(NewDescriptionValuePair(
      "Initialization time",
      base::Int64ToString(gpu_info.initialization_time.InMilliseconds())));
  basic_info->Append(NewDescriptionValuePair(
      "Sandboxed",
      Value::CreateBooleanValue(gpu_info.sandboxed)));
  basic_info->Append(NewDescriptionValuePair(
      "GPU0", GPUDeviceToString(gpu_info.gpu)));
  for (size_t i = 0; i < gpu_info.secondary_gpus.size(); ++i) {
    basic_info->Append(NewDescriptionValuePair(
        base::StringPrintf("GPU%d", static_cast<int>(i + 1)),
        GPUDeviceToString(gpu_info.secondary_gpus[i])));
  }
  basic_info->Append(NewDescriptionValuePair(
      "Optimus", Value::CreateBooleanValue(gpu_info.optimus)));
  basic_info->Append(NewDescriptionValuePair(
      "AMD switchable", Value::CreateBooleanValue(gpu_info.amd_switchable)));
  basic_info->Append(NewDescriptionValuePair("Driver vendor",
                                             gpu_info.driver_vendor));
  basic_info->Append(NewDescriptionValuePair("Driver version",
                                             gpu_info.driver_version));
  basic_info->Append(NewDescriptionValuePair("Driver date",
                                             gpu_info.driver_date));
  basic_info->Append(NewDescriptionValuePair("Pixel shader version",
                                             gpu_info.pixel_shader_version));
  basic_info->Append(NewDescriptionValuePair("Vertex shader version",
                                             gpu_info.vertex_shader_version));
  basic_info->Append(NewDescriptionValuePair("Machine model",
                                             gpu_info.machine_model));
  basic_info->Append(NewDescriptionValuePair("GL version",
                                             gpu_info.gl_version));
  basic_info->Append(NewDescriptionValuePair("GL_VENDOR",
                                             gpu_info.gl_vendor));
  basic_info->Append(NewDescriptionValuePair("GL_RENDERER",
                                             gpu_info.gl_renderer));
  basic_info->Append(NewDescriptionValuePair("GL_VERSION",
                                             gpu_info.gl_version_string));
  basic_info->Append(NewDescriptionValuePair("GL_EXTENSIONS",
                                             gpu_info.gl_extensions));

  DictionaryValue* info = new DictionaryValue();
  info->Set("basic_info", basic_info);

#if defined(OS_WIN)
  ListValue* perf_info = new ListValue();
  perf_info->Append(NewDescriptionValuePair(
      "Graphics",
      base::StringPrintf("%.1f", gpu_info.performance_stats.graphics)));
  perf_info->Append(NewDescriptionValuePair(
      "Gaming",
      base::StringPrintf("%.1f", gpu_info.performance_stats.gaming)));
  perf_info->Append(NewDescriptionValuePair(
      "Overall",
      base::StringPrintf("%.1f", gpu_info.performance_stats.overall)));
  info->Set("performance_info", perf_info);

  Value* dx_info;
  if (gpu_info.dx_diagnostics.children.size())
    dx_info = DxDiagNodeToList(gpu_info.dx_diagnostics);
  else
    dx_info = Value::CreateNullValue();
  info->Set("diagnostics", dx_info);
#endif

  return info;
}

// Determine if accelerated-2d-canvas is supported, which depends on whether
// lose_context could happen and whether skia is the backend.
bool SupportsAccelerated2dCanvas() {
  if (GpuDataManager::GetInstance()->GetGPUInfo().can_lose_context)
    return false;
#if defined(USE_SKIA)
  return true;
#else
  return false;
#endif
}

Value* GetFeatureStatus() {
  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  bool gpu_access_blocked = !GpuDataManager::GetInstance()->GpuAccessAllowed();

  uint32 flags = GpuDataManager::GetInstance()->GetBlacklistedFeatures();
  DictionaryValue* status = new DictionaryValue();

  const GpuFeatureInfo kGpuFeatureInfo[] = {
      {
          "2d_canvas",
          flags & content::GPU_FEATURE_TYPE_ACCELERATED_2D_CANVAS,
          command_line.HasSwitch(switches::kDisableAccelerated2dCanvas) ||
          !SupportsAccelerated2dCanvas(),
          "Accelerated 2D canvas is unavailable: either disabled at the command"
          " line or not supported by the current system.",
          true
      },
      {
          "compositing",
          flags & content::GPU_FEATURE_TYPE_ACCELERATED_COMPOSITING,
          command_line.HasSwitch(switches::kDisableAcceleratedCompositing),
          "Accelerated compositing has been disabled, either via about:flags or"
          " command line. This adversely affects performance of all hardware"
          " accelerated features.",
          true
      },
      {
          "3d_css",
          flags & (content::GPU_FEATURE_TYPE_ACCELERATED_COMPOSITING |
                   content::GPU_FEATURE_TYPE_3D_CSS),
          command_line.HasSwitch(switches::kDisableAcceleratedLayers),
          "Accelerated layers have been disabled at the command line.",
          false
      },
      {
          "css_animation",
          flags & (content::GPU_FEATURE_TYPE_ACCELERATED_COMPOSITING |
                   content::GPU_FEATURE_TYPE_3D_CSS),
          command_line.HasSwitch(cc::switches::kDisableThreadedAnimation) ||
          command_line.HasSwitch(switches::kDisableAcceleratedCompositing) ||
          command_line.HasSwitch(switches::kDisableAcceleratedLayers),
          "Accelerated CSS animation has been disabled at the command line.",
          true
      },
      {
          "webgl",
          flags & content::GPU_FEATURE_TYPE_WEBGL,
#if defined(OS_ANDROID)
          !command_line.HasSwitch(switches::kEnableExperimentalWebGL),
#else
          command_line.HasSwitch(switches::kDisableExperimentalWebGL),
#endif
          "WebGL has been disabled, either via about:flags or command line.",
          false
      },
      {
          "multisampling",
          flags & content::GPU_FEATURE_TYPE_MULTISAMPLING,
          command_line.HasSwitch(switches::kDisableGLMultisampling),
          "Multisampling has been disabled, either via about:flags or command"
          " line.",
          false
      },
      {
          "flash_3d",
          flags & content::GPU_FEATURE_TYPE_FLASH3D,
          command_line.HasSwitch(switches::kDisableFlash3d),
          "Using 3d in flash has been disabled, either via about:flags or"
          " command line.",
          false
      },
      {
          "flash_stage3d",
          flags & content::GPU_FEATURE_TYPE_FLASH_STAGE3D,
          command_line.HasSwitch(switches::kDisableFlashStage3d),
          "Using Stage3d in Flash has been disabled, either via about:flags or"
          " command line.",
          false
      },
      {
          "texture_sharing",
          flags & content::GPU_FEATURE_TYPE_TEXTURE_SHARING,
          command_line.HasSwitch(switches::kDisableImageTransportSurface),
          "Sharing textures between processes has been disabled, either via"
          " about:flags or command line.",
          false
      },
      {
          "video_decode",
          flags & content::GPU_FEATURE_TYPE_ACCELERATED_VIDEO_DECODE,
          command_line.HasSwitch(switches::kDisableAcceleratedVideoDecode),
          "Accelerated video decode has been disabled, either via about:flags"
          " or command line.",
          true
      },
      {
          "video",
          flags & content::GPU_FEATURE_TYPE_ACCELERATED_VIDEO,
          command_line.HasSwitch(switches::kDisableAcceleratedVideo) ||
          command_line.HasSwitch(switches::kDisableAcceleratedCompositing),
          "Accelerated video presentation has been disabled, either via"
          " about:flags or command line.",
          true
      },
      {
          "panel_fitting",
          flags & content::GPU_FEATURE_TYPE_PANEL_FITTING,
#if defined(OS_CHROMEOS)
          command_line.HasSwitch(ash::switches::kAshDisablePanelFitting),
#else
          true,
#endif
          "Panel fitting is unavailable, either disabled at the command"
          " line or not supported by the current system.",
          false
      }
  };
  const size_t kNumFeatures = sizeof(kGpuFeatureInfo) / sizeof(GpuFeatureInfo);

  // Build the feature_status field.
  {
    ListValue* feature_status_list = new ListValue();

    for (size_t i = 0; i < kNumFeatures; ++i) {
      std::string status;
      if (kGpuFeatureInfo[i].disabled) {
        status = "disabled";
        if (kGpuFeatureInfo[i].name == "css_animation") {
          status += "_software_animated";
        } else {
          if (kGpuFeatureInfo[i].fallback_to_software)
            status += "_software";
          else
            status += "_off";
        }
      } else if (GpuDataManager::GetInstance()->ShouldUseSoftwareRendering()) {
        status = "unavailable_software";
      } else if (kGpuFeatureInfo[i].blocked ||
                 gpu_access_blocked) {
        status = "unavailable";
        if (kGpuFeatureInfo[i].fallback_to_software)
          status += "_software";
        else
          status += "_off";
      } else {
        status = "enabled";
        if (kGpuFeatureInfo[i].name == "webgl" &&
            (command_line.HasSwitch(switches::kDisableAcceleratedCompositing) ||
             (flags & content::GPU_FEATURE_TYPE_ACCELERATED_COMPOSITING)))
          status += "_readback";
        bool has_thread = content::IsThreadedCompositingEnabled();
        if (kGpuFeatureInfo[i].name == "compositing") {
          bool force_compositing =
              content::IsForceCompositingModeEnabled();
          if (force_compositing)
            status += "_force";
          if (has_thread)
            status += "_threaded";
        }
        if (kGpuFeatureInfo[i].name == "css_animation") {
          if (has_thread)
            status = "accelerated_threaded";
          else
            status = "accelerated";
        }
      }
      feature_status_list->Append(
          NewStatusValue(kGpuFeatureInfo[i].name.c_str(), status.c_str()));
    }
    content::GPUInfo gpu_info = GpuDataManager::GetInstance()->GetGPUInfo();
    if (gpu_info.secondary_gpus.size() > 0 ||
        gpu_info.optimus || gpu_info.amd_switchable) {
      std::string gpu_switching;
      switch (GpuDataManager::GetInstance()->GetGpuSwitchingOption()) {
        case content::GPU_SWITCHING_OPTION_AUTOMATIC:
          gpu_switching = "gpu_switching_automatic";
          break;
        case content::GPU_SWITCHING_OPTION_FORCE_DISCRETE:
          gpu_switching = "gpu_switching_force_discrete";
          break;
        case content::GPU_SWITCHING_OPTION_FORCE_INTEGRATED:
          gpu_switching = "gpu_switching_force_integrated";
          break;
        default:
          break;
      }
      feature_status_list->Append(
          NewStatusValue("gpu_switching", gpu_switching.c_str()));
    }
    status->Set("featureStatus", feature_status_list);
  }

  // Build the problems list.
  {
    ListValue* problem_list =
        GpuDataManager::GetInstance()->GetBlacklistReasons();

    if (gpu_access_blocked) {
      DictionaryValue* problem = new DictionaryValue();
      problem->SetString("description",
          "GPU process was unable to boot. Access to GPU disallowed.");
      problem->Set("crBugs", new ListValue());
      problem->Set("webkitBugs", new ListValue());
      problem_list->Append(problem);
    }

    for (size_t i = 0; i < kNumFeatures; ++i) {
      if (kGpuFeatureInfo[i].disabled) {
        DictionaryValue* problem = new DictionaryValue();
        problem->SetString(
            "description", kGpuFeatureInfo[i].disabled_description);
        problem->Set("crBugs", new ListValue());
        problem->Set("webkitBugs", new ListValue());
        problem_list->Append(problem);
      }
    }

    status->Set("problems", problem_list);
  }

  return status;
}

}  // namespace

void PrintGpuInfo() {
  // Get GPU Info.
  scoped_ptr<base::DictionaryValue> gpu_info_val(
      GpuInfoAsDictionaryValue());

  // Add in blacklisting features
  Value* feature_status = GetFeatureStatus();
  if (feature_status)
    gpu_info_val->Set("featureStatus", feature_status);

  LOG(ERROR) << *gpu_info_val;
}

void PrintClientInfo() {
  DictionaryValue* dict = new DictionaryValue();

  dict->SetString("operating_system",
                  base::SysInfo::OperatingSystemName() + " " +
                  base::SysInfo::OperatingSystemVersion());
  dict->SetString("angle_revision", base::UintToString(BUILD_REVISION));
#if defined(USE_SKIA)
  dict->SetString("graphics_backend", "Skia");
#else
  dict->SetString("graphics_backend", "Core Graphics");
#endif
  dict->SetString("blacklist_version",
      GpuDataManager::GetInstance()->GetBlacklistVersion());

  LOG(ERROR) << *dict;

  delete dict;
}
