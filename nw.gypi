# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'nw_product_name': 'nwlegacy',
    'mac_strip_release': 1,
    'nw_gen_path': '<(SHARED_INTERMEDIATE_DIR)/nw',
    'nw_id_script_base': 'commit_id.py',
    'nw_id_script': '<(nw_gen_path)/<(nw_id_script_base)',
    'nw_id_header_base': 'commit.h',
    'nw_id_header': '<(nw_gen_path)/id/<(nw_id_header_base)',
    'nw_use_commit_id%': '<!(python <(DEPTH)/content/nw/tools/<(nw_id_script_base) check ..)',
  },
  'target_defaults': {
    'msvs_settings': {
      'VCLinkerTool': {
        'AdditionalDependencies': [
            'runtimeobject.lib',
        ],
        'DelayLoadDLLs': [
            'API-MS-WIN-CORE-WINRT-ERROR-L1-1-0.DLL',
            'API-MS-WIN-CORE-WINRT-L1-1-0.DLL',
            'API-MS-WIN-CORE-WINRT-STRING-L1-1-0.DLL',
        ],
      },
    },
    'configurations': {
      'Release_Base': {
        # Set flags to unconditionally optimize chrome_frame_launcher.exe
        # for release builds.
        'msvs_settings': {
          'VCLibrarianTool': {
            'AdditionalOptions': ['/ltcg', '/expectedoutputsize:600000000'],
          },
        },
      },
    },
  },
  'targets': [
    {
      'target_name': 'nw_lib',
      'type': 'static_library',
      'defines!': ['CONTENT_IMPLEMENTATION'],
      'defines': ['BREAKPAD_IMPLEMENTATION'],
      'variables': {
        'chromium_code': 1,
      },
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_prefs_test_support',
        '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(DEPTH)/chrome/common/extensions/api/api.gyp:chrome_api',
        '<(DEPTH)/components/components.gyp:app_modal',
        '<(DEPTH)/components/components.gyp:autofill_content_renderer',
        '<(DEPTH)/components/components.gyp:keyed_service_content',
        '<(DEPTH)/components/components_resources.gyp:components_resources',
        '<(DEPTH)/components/components.gyp:autofill_content_browser',
        '<(DEPTH)/content/content.gyp:content_app_browser',
        '<(DEPTH)/content/content.gyp:content_browser',
        '<(DEPTH)/content/content.gyp:content_common',
        '<(DEPTH)/content/content.gyp:content_gpu',
        '<(DEPTH)/content/content.gyp:content_plugin',
        '<(DEPTH)/content/content.gyp:content_ppapi_plugin',
        '<(DEPTH)/content/content.gyp:content_renderer',
        '<(DEPTH)/content/content.gyp:content_utility',
        #        '<(DEPTH)/content/content.gyp:content_worker',
        '<(DEPTH)/content/app/resources/content_resources.gyp:content_resources',
        '<(DEPTH)/device/core/core.gyp:device_core',
        '<(DEPTH)/device/hid/hid.gyp:device_hid',
        '<(DEPTH)/extensions/extensions.gyp:extensions_common_constants',
        '<(DEPTH)/ipc/ipc.gyp:ipc',
        '<(DEPTH)/media/media.gyp:media',
        '<(DEPTH)/net/net.gyp:net_with_v8',
        '<(DEPTH)/net/net.gyp:net_resources',
        '<(DEPTH)/printing/printing.gyp:printing',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/third_party/node/node.gyp:node',
        '<(DEPTH)/ui/base/ui_base.gyp:ui_base',
        '<(DEPTH)/ui/resources/ui_resources.gyp:ui_resources',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
        '<(DEPTH)/third_party/libyuv/libyuv.gyp:libyuv',
        '<(DEPTH)/third_party/webrtc/modules/modules.gyp:desktop_capture',
        '<(DEPTH)/third_party/zlib/zlib.gyp:minizip',
        '<(DEPTH)/third_party/WebKit/public/blink.gyp:blink',
        '<(DEPTH)/extensions/browser/api/api_registration.gyp:extensions_api_registration',
        '<(DEPTH)/extensions/common/api/api.gyp:extensions_api',
        '<(DEPTH)/extensions/extensions.gyp:extensions_browser',
        '<(DEPTH)/extensions/extensions.gyp:extensions_common',
        '<(DEPTH)/extensions/extensions.gyp:extensions_renderer',
        '<(DEPTH)/extensions/extensions.gyp:extensions_shell_and_test_pak',
        '<(DEPTH)/extensions/extensions.gyp:extensions_utility',
        '<(DEPTH)/extensions/extensions_resources.gyp:extensions_resources',
        '<(DEPTH)/components/components_strings.gyp:components_strings',
        '<(DEPTH)/components/components.gyp:pdf_renderer',
        '<(DEPTH)/ppapi/ppapi_internal.gyp:ppapi_host',
        '<(DEPTH)/ppapi/ppapi_internal.gyp:ppapi_proxy',
        '<(DEPTH)/ppapi/ppapi_internal.gyp:ppapi_ipc',
        '<(DEPTH)/ppapi/ppapi_internal.gyp:ppapi_shared',
        'commit_id',
      ],
      'include_dirs': [
        '<(DEPTH)',
        '<(DEPTH)/v8',
        '<(DEPTH)/third_party',
        '<(DEPTH)/third_party/WebKit/Source',
        '<(DEPTH)/third_party/WebKit/public/web',
        '<(DEPTH)/breakpad/src',
        '<(SHARED_INTERMEDIATE_DIR)/blink',
        '<(SHARED_INTERMEDIATE_DIR)/blink/bindings/core/v8/',
        '<(SHARED_INTERMEDIATE_DIR)/chrome',
      ],
      'cflags_cc': [
        '-Wno-error=c++0x-compat',
      ],
      'sources': [
        '<(DEPTH)/chrome/browser/chrome_process_finder_win.cc',
        '<(DEPTH)/chrome/browser/chrome_process_finder_win.h',
        #'<(DEPTH)/chrome/common/child_process_logging.h',
        #'<(DEPTH)/chrome/common/child_process_logging_mac.mm',
        #'<(DEPTH)/chrome/common/child_process_logging_posix.cc',
        #'<(DEPTH)/chrome/common/child_process_logging_win.cc',
        '<(DEPTH)/chrome/common/crash_keys.cc',
        '<(DEPTH)/chrome/common/env_vars.cc',
        '<(DEPTH)/chrome/browser/crash_upload_list.cc',
        '<(DEPTH)/chrome/browser/upload_list.cc',
        '<(DEPTH)/chrome/browser/platform_util_linux.cc',
        '<(DEPTH)/chrome/browser/platform_util_aura.cc',
        '<(DEPTH)/chrome/browser/platform_util_mac.mm',
        '<(DEPTH)/chrome/browser/platform_util_win.cc',
        '<(DEPTH)/chrome/browser/platform_util.h',
        '<(DEPTH)/chrome/browser/process_singleton_posix.cc',
        '<(DEPTH)/chrome/browser/process_singleton_win.cc',
        '<(DEPTH)/chrome/browser/process_singleton.h',
        '<(DEPTH)/chrome/browser/status_icons/status_icon.cc',
        '<(DEPTH)/chrome/browser/status_icons/status_icon.h',
        '<(DEPTH)/chrome/browser/status_icons/status_icon_observer.h',
        '<(DEPTH)/chrome/browser/status_icons/status_tray.cc',
        '<(DEPTH)/chrome/browser/status_icons/status_tray.h',
        '<(DEPTH)/chrome/browser/ui/cocoa/custom_frame_view.h',
        '<(DEPTH)/chrome/browser/ui/cocoa/custom_frame_view.mm',
        '<(DEPTH)/chrome/browser/ui/base_window.h',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_icon_linux_wrapper.cc',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_icon_linux_wrapper.h',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_icon_win.cc',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_icon_win.h',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_linux.cc',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_linux.h',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_state_changer_win.cc',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_state_changer_win.h',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_win.cc',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_win.h',
        '<(DEPTH)/chrome/common/chrome_constants.cc',
        '<(DEPTH)/chrome/common/chrome_constants.h',
        '<(DEPTH)/chrome/common/chrome_switches.cc',
        '<(DEPTH)/chrome/common/chrome_switches.h',
        '<(DEPTH)/chrome/renderer/pepper/chrome_pdf_print_client.cc',
        '<(DEPTH)/chrome/renderer/pepper/chrome_pdf_print_client.h',
        '<(DEPTH)/chrome/renderer/pepper/chrome_renderer_pepper_host_factory.cc',
        '<(DEPTH)/chrome/renderer/pepper/chrome_renderer_pepper_host_factory.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_drm_renderer_host.cc',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_drm_renderer_host.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_font_file_host.cc',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_font_file_host.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_fullscreen_host.cc',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_fullscreen_host.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_menu_host.cc',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_menu_host.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_renderer_host.cc',
        '<(DEPTH)/chrome/renderer/pepper/pepper_flash_renderer_host.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_helper.cc',
        '<(DEPTH)/chrome/renderer/pepper/pepper_helper.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_shared_memory_message_filter.cc',
        '<(DEPTH)/chrome/renderer/pepper/pepper_shared_memory_message_filter.h',
        '<(DEPTH)/chrome/renderer/pepper/pepper_uma_host.h',
        '<(DEPTH)/extensions/common/draggable_region.cc',
        '<(DEPTH)/extensions/common/draggable_region.h',
        '<(DEPTH)/extensions/renderer/static_v8_external_one_byte_string_resource.cc',
        '<(DEPTH)/extensions/renderer/static_v8_external_one_byte_string_resource.h',
        '<(DEPTH)/third_party/zlib/google/zip.cc',
        '<(DEPTH)/third_party/zlib/google/zip.h',
        '<(DEPTH)/third_party/zlib/google/zip_reader.cc',
        '<(DEPTH)/third_party/zlib/google/zip_reader.h',
        '<(DEPTH)/third_party/zlib/google/zip_internal.cc',
        '<(DEPTH)/third_party/zlib/google/zip_internal.h',
        'src/api/api_messages.cc',
        'src/api/api_messages.h',
        'src/api/app/app.cc',
        'src/api/app/app.h',
        'src/api/bindings_common.cc',
        'src/api/bindings_common.h',
        'src/api/base/base.cc',
        'src/api/base/base.h',
        'src/api/clipboard/clipboard.cc',
        'src/api/clipboard/clipboard.h',
        'src/api/dispatcher.cc',
        'src/api/dispatcher.h',
        'src/api/dispatcher_bindings.cc',
        'src/api/dispatcher_bindings.h',
        'src/api/dispatcher_bindings_mac.mm',
        'src/api/dispatcher_host.cc',
        'src/api/dispatcher_host.h',
        'src/api/event/event.h',
        'src/api/event/event.cc',
        'src/api/screen/desktop_capture_api.h',
        'src/api/screen/desktop_capture_api.cc',
        '<(DEPTH)/chrome/browser/media/desktop_media_list.h',
        '<(DEPTH)/chrome/browser/media/desktop_media_list_observer.h',
        '<(DEPTH)/chrome/browser/media/desktop_media_picker.h',
        '<(DEPTH)/chrome/browser/media/native_desktop_media_list.h',
        '<(DEPTH)/chrome/browser/media/native_desktop_media_list.cc',
        'src/api/screen/screen.h',
        'src/api/screen/screen.cc',
        'src/api/window_bindings.cc',
        'src/api/window_bindings.h',
        'src/api/v8_internal_helper.cc',
        'src/api/menu/menu.cc',
        'src/api/menu/menu.h',
        'src/api/menu/menu_delegate.cc',
        'src/api/menu/menu_delegate.h',
        'src/api/menu/menu_delegate_mac.h',
        'src/api/menu/menu_delegate_mac.mm',
        'src/api/menu/menu_mac.mm',
        'src/api/menu/menu_views.cc',
        'src/api/menuitem/menuitem.cc',
        'src/api/menuitem/menuitem.h',
        'src/api/menuitem/menuitem_mac.mm',
        'src/api/menuitem/menuitem_views.cc',
        'src/api/menuitem/menuitem_delegate_mac.h',
        'src/api/menuitem/menuitem_delegate_mac.mm',
        'src/api/shell/shell.cc',
        'src/api/shell/shell.h',
        'src/api/shortcut/global_shortcut_listener.cc',
        'src/api/shortcut/global_shortcut_listener.h',
        'src/api/shortcut/global_shortcut_listener_mac.h',
        'src/api/shortcut/global_shortcut_listener_mac.mm',
        'src/api/shortcut/global_shortcut_listener_x11.cc',
        'src/api/shortcut/global_shortcut_listener_x11.h',
        'src/api/shortcut/global_shortcut_listener_win.cc',
        'src/api/shortcut/global_shortcut_listener_win.h',
        'src/api/shortcut/shortcut.cc',
        'src/api/shortcut/shortcut.h',
        'src/api/shortcut/shortcut_constants.cc',
        'src/api/shortcut/shortcut_constants.h',
        'src/api/tray/tray.cc',
        'src/api/tray/tray.h',
        'src/api/tray/tray_aura.cc',
        'src/api/tray/tray_mac.mm',
        'src/api/window/window.cc',
        'src/api/window/window.h',
        'src/browser/app_controller_mac.h',
        'src/browser/app_controller_mac.mm',
        'src/browser/autofill_popup_base_view_cocoa.h',
        'src/browser/autofill_popup_base_view_cocoa.mm',
        'src/browser/autofill_popup_view_cocoa.h',
        'src/browser/autofill_popup_view_cocoa.mm',
        'src/browser/autofill_popup_view_bridge.h',
        'src/browser/autofill_popup_view_bridge.mm',
        '<(DEPTH)/chrome/browser/ui/autofill/autofill_popup_controller_impl.cc',
        '<(DEPTH)/chrome/browser/ui/autofill/autofill_popup_controller_impl.h',
        'src/browser/browser_view_layout.cc',
        'src/browser/browser_view_layout.h',
        'src/browser/nw_autofill_client.cc',
        'src/browser/nw_autofill_client.h',
        'src/browser/nw_autofill_client_mac.mm',
        'src/browser/capture_page_helper.h',
        'src/browser/capture_page_helper.cc',
        'src/browser/color_chooser_aura.cc',
        'src/browser/color_chooser_aura.h',
        'src/browser/color_chooser_win.cc',
        'src/browser/color_chooser_mac.mm',
        'src/browser/chrome_event_processing_window.mm',
        'src/browser/chrome_event_processing_window.h',
        'src/browser/file_select_helper.cc',
        'src/browser/file_select_helper.h',
        'src/browser/file_select_helper_mac.mm',
        'src/browser/menubar_controller.cc',
        'src/browser/menubar_controller.h',
        'src/browser/menubar_view.cc',
        'src/browser/menubar_view.h',
        'src/browser/native_window.cc',
        'src/browser/native_window.h',
        'src/browser/native_window_helper_mac.h',
        'src/browser/native_window_mac.h',
        'src/browser/native_window_mac.mm',
        'src/browser/native_window_toolbar_aura.cc',
        'src/browser/native_window_toolbar_aura.h',
        'src/browser/native_window_aura.cc',
        'src/browser/native_window_aura.h',
        'src/browser/net_disk_cache_remover.cc',
        'src/browser/net_disk_cache_remover.h',
        'src/browser/nw_form_database_service.cc',
        'src/browser/nw_form_database_service.h',
        'src/browser/popup_controller_common.cc',
        'src/browser/popup_controller_common.h',
        'src/browser/shell_component_extension_resource_manager.cc',
        'src/browser/shell_component_extension_resource_manager.h',
        '<(DEPTH)/chrome/browser/printing/print_job.cc',
        '<(DEPTH)/chrome/browser/printing/print_job.h',
        '<(DEPTH)/chrome/browser/printing/print_job_manager.cc',
        '<(DEPTH)/chrome/browser/printing/print_job_manager.h',
        '<(DEPTH)/chrome/browser/printing/print_job_worker.cc',
        '<(DEPTH)/chrome/browser/printing/print_job_worker.h',
        '<(DEPTH)/chrome/browser/printing/print_job_worker_owner.h',
        '<(DEPTH)/chrome/browser/printing/print_job_worker_owner.cc',
        'src/browser/printing/printing_message_filter.cc',
        'src/browser/printing/printing_message_filter.h',
        '<(DEPTH)/chrome/browser/printing/printer_query.cc',
        '<(DEPTH)/chrome/browser/printing/printer_query.h',
        '<(DEPTH)/chrome/browser/printing/print_view_manager.cc',
        '<(DEPTH)/chrome/browser/printing/print_view_manager.h',
        '<(DEPTH)/chrome/browser/printing/print_view_manager_base.cc',
        '<(DEPTH)/chrome/browser/printing/print_view_manager_base.h',
        '<(DEPTH)/chrome/browser/printing/print_view_manager_basic.cc',
        '<(DEPTH)/chrome/browser/printing/print_view_manager_basic.h',
        'src/browser/shell_application_mac.h',
        'src/browser/shell_application_mac.mm',
        'src/browser/shell_devtools_manager_delegate.cc',
        'src/browser/shell_devtools_manager_delegate.h',
        'src/browser/shell_download_manager_delegate.cc',
        'src/browser/shell_download_manager_delegate.h',
        'src/browser/shell_download_manager_delegate_win.cc',
        'src/browser/shell_download_manager_delegate_mac.mm',
        'src/browser/shell_extension_system.cc',
        'src/browser/shell_extension_system.h',
        'src/browser/shell_extension_system_factory.cc',
        'src/browser/shell_extension_system_factory.h',
        'src/browser/shell_extensions_api_client.cc',
        'src/browser/shell_extensions_api_client.h',
        'src/browser/shell_extensions_browser_client.cc',
        'src/browser/shell_extensions_browser_client.h',
        'src/browser/shell_extension_host_delegate.cc',
        'src/browser/shell_extension_host_delegate.h',
        'src/browser/shell_extension_web_contents_observer.cc',
        'src/browser/shell_extension_web_contents_observer.h',
        'src/browser/shell_speech_recognition_manager_delegate.cc',
        'src/browser/shell_speech_recognition_manager_delegate.h',
        'src/browser/shell_web_view_guest_delegate.cc',
        'src/browser/shell_web_view_guest_delegate.h',
        'src/browser/shell_display_info_provider.cc',
        'src/browser/shell_display_info_provider.h',
        'src/browser/shell_runtime_api_delegate.cc',
        'src/browser/shell_runtime_api_delegate.h',
        'src/browser/media_capture_util.cc',
        'src/browser/media_capture_util.h',
        'src/browser/shell_javascript_dialog_creator.cc',
        'src/browser/shell_javascript_dialog_creator.h',
#        'src/browser/shell_javascript_dialog_gtk.cc',
        'src/browser/shell_javascript_dialog_mac.mm',
        'src/browser/shell_javascript_dialog_win.cc',
        'src/browser/shell_javascript_dialog.h',
 #       'src/browser/shell_login_dialog_gtk.cc',
        'src/browser/shell_login_dialog_mac.mm',
        'src/browser/shell_login_dialog_views.cc',
        'src/browser/shell_login_dialog.cc',
        'src/browser/shell_login_dialog.h',
        'src/browser/shell_resource_dispatcher_host_delegate.cc',
        'src/browser/shell_resource_dispatcher_host_delegate.h',
        'src/browser/shell_toolbar_delegate_mac.h',
        'src/browser/shell_toolbar_delegate_mac.mm',
        'src/browser/standard_menus_mac.h',
        'src/browser/standard_menus_mac.mm',
        'src/browser/chrome_crash_reporter_client.cc',
        'src/browser/chrome_crash_reporter_client.h',
        'src/browser/chrome_crash_reporter_client_mac.mm',
        'src/common/print_messages.cc',
        'src/common/print_messages.h',
        'src/common/shell_switches.cc',
        'src/common/shell_switches.h',
        'src/common/shell_extensions_client.cc',
        'src/common/shell_extensions_client.h',
        'src/breakpad_linux_impl.h',
        'src/geolocation/shell_access_token_store.cc',
        'src/geolocation/shell_access_token_store.h',
#        'src/browser/media_capture_devices_dispatcher.cc',
#        'src/browser/media_capture_devices_dispatcher.h',
#        'src/browser/media_stream_devices_controller.cc',
#        'src/browser/media_stream_devices_controller.h',
        'src/net/app_protocol_handler.cc',
        'src/net/app_protocol_handler.h',
#        'src/net/clear_on_exit_policy.h',
#        'src/net/clear_on_exit_policy.cc',
        'src/net/resource_request_job.cc',
        'src/net/resource_request_job.h',
        'src/net/shell_network_delegate.cc',
        'src/net/shell_network_delegate.h',
        'src/net/shell_url_request_context_getter.cc',
        'src/net/shell_url_request_context_getter.h',
        'src/nw_protocol_handler.cc',
        'src/nw_protocol_handler.h',
        'src/nw_package.cc',
        'src/nw_package.h',
        'src/nw_version.h',
        'src/paths_mac.h',
        'src/paths_mac.mm',
        'src/renderer/common/render_messages.cc',
        'src/renderer/common/render_messages.h',
        'src/renderer/pepper_uma_host.cc',
        'src/renderer/prerenderer/prerenderer_client.cc',
        'src/renderer/prerenderer/prerenderer_client.h',
        'src/renderer/printing/print_web_view_helper.cc',
        'src/renderer/printing/print_web_view_helper.h',
        'src/renderer/printing/print_web_view_helper_linux.cc',
        'src/renderer/printing/print_web_view_helper_mac.mm',
        'src/renderer/printing/print_web_view_helper_pdf_win.cc',
        'src/renderer/nw_render_view_observer.cc',
        'src/renderer/nw_render_view_observer.h',
        'src/renderer/shell_content_renderer_client.cc',
        'src/renderer/shell_content_renderer_client.h',
        'src/renderer/shell_extensions_renderer_client.cc',
        'src/renderer/shell_extensions_renderer_client.h',
        'src/renderer/shell_render_process_observer.cc',
        'src/renderer/shell_render_process_observer.h',
        'src/nw_shell.cc',
        'src/nw_shell.h',
        'src/nw_notification_manager.h',
        'src/nw_notification_manager.cc',
        'src/nw_notification_manager_win.h',
        'src/nw_notification_manager_win.cc',		
        'src/nw_notification_manager_toast_win.h',
        'src/nw_notification_manager_toast_win.cc',		
        'src/nw_notification_manager_mac.h',
        'src/nw_notification_manager_mac.mm',
        'src/nw_notification_manager_linux.h',
        'src/nw_notification_manager_linux.cc',
        'src/shell_browser_context.cc',
        'src/shell_browser_context.h',
        'src/shell_browser_main.cc',
        'src/shell_browser_main.h',
        'src/shell_browser_main_parts.cc',
        'src/shell_browser_main_parts.h',
        'src/shell_browser_main_parts_mac.mm',
        'src/shell_content_browser_client.cc',
        'src/shell_content_browser_client.h',
        'src/shell_content_client.cc',
        'src/shell_content_client.h',
        'src/shell_devtools_frontend.cc',
        'src/shell_devtools_frontend.h',
        'src/shell_main_delegate.cc',
        'src/shell_main_delegate.h',
        'src/shell_quota_permission_context.cc',
        'src/shell_quota_permission_context.h',
        '<(SHARED_INTERMEDIATE_DIR)/content/grit/nw_component_resources_map.cc',
        'src/browser/pepper/chrome_browser_pepper_host_factory.cc',
        'src/browser/pepper/chrome_browser_pepper_host_factory.h',
        'src/browser/pepper/pepper_broker_message_filter.cc',
        'src/browser/pepper/pepper_broker_message_filter.h',
        'src/browser/pepper/pepper_flash_browser_host.cc',
        'src/browser/pepper/pepper_flash_browser_host.h',
        'src/browser/pepper/pepper_flash_clipboard_message_filter.cc',
        'src/browser/pepper/pepper_flash_clipboard_message_filter.h',
        'src/browser/pepper/pepper_isolated_file_system_message_filter.cc',
        'src/browser/pepper/pepper_isolated_file_system_message_filter.h',
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '2',  # Set /SUBSYSTEM:WINDOWS
        },
      },
      'msvs_precompiled_header': '',
      'msvs_precompiled_source': '',
      'configurations': {
        'Debug': {
          'defines': [ 'DEBUG' ],
        },
      },
      'conditions': [
        ['OS=="win" and win_use_allocator_shim==1', {
          'dependencies': [
            '<(DEPTH)/base/allocator/allocator.gyp:allocator',
          ],
        }],
        ['os_posix == 1 and OS != "mac"', {
          'dependencies': [
            '../components/components.gyp:breakpad_host',
          ],
        }],
        ['use_aura==1', {
          'sources': [
            '<(DEPTH)/chrome/browser/ui/views/desktop_media_picker_views.cc',
            '<(DEPTH)/chrome/browser/ui/views/desktop_media_picker_views.h',
            '<(DEPTH)/chrome/browser/ui/views/web_contents_modal_dialog_manager_views.cc',
            'src/browser/login_view.cc',
            'src/browser/login_view.h',
          ],
          'dependencies': [
            '<(DEPTH)/components/components.gyp:constrained_window',
            '<(DEPTH)/components/components.gyp:web_modal',
            '<(DEPTH)/ui/resources/ui_resources.gyp:ui_resources',
            '<(DEPTH)/ui/views/views.gyp:views',
            '<(DEPTH)/ui/views/views.gyp:views_test_support',
            '<(DEPTH)/ui/views/controls/webview/webview.gyp:webview',
          ],
        }],
        ['(os_posix==1 and OS != "mac" and use_allocator!="none")', {
          'dependencies': [
            # This is needed by content/app/content_main_runner.cc
            '<(DEPTH)/base/allocator/allocator.gyp:allocator',
          ],
        }],
        ['OS=="win" or OS=="linux"', {
          'sources': [
            'src/browser/autofill_popup_base_view.cc',
            'src/browser/autofill_popup_base_view.h',
            'src/browser/autofill_popup_view_views.cc',
            'src/browser/autofill_popup_view_views.h',
          ],
        }],
        ['OS=="win"', {
          'dependencies': [
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad_handler',
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad_sender',
            '<(DEPTH)/components/components.gyp:crash_component',
          ],
        }],
        ['os_posix==1 and OS != "mac" and OS != "ios"', {
          'dependencies': [
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad_client',
            '<(DEPTH)/components/components.gyp:crash_component',
          ],
        }],
        ['OS=="linux"', {
          'dependencies': [
            '<(DEPTH)/chrome/browser/ui/libgtk2ui/libgtk2ui.gyp:gtk2ui',
          ],
          'sources': [
            'src/browser/shell_download_manager_delegate_gtk.cc',
            '<(DEPTH)/chrome/browser/ui/views/chrome_javascript_native_dialog_factory_views.cc',
            '<(DEPTH)/chrome/browser/ui/views/javascript_app_modal_dialog_views_x11.cc',
            '<(DEPTH)/chrome/browser/ui/views/javascript_app_modal_dialog_views_x11.h',
            '<(DEPTH)/chrome/browser/ui/views/javascript_app_modal_event_blocker_x11.cc',
            '<(DEPTH)/chrome/browser/ui/views/javascript_app_modal_event_blocker_x11.h',
            'src/browser/nw_constrained_window_views_client.cc',
            'src/browser/nw_constrained_window_views_client.h',
          ],
        }],
        ['OS == "mac"', {
          'sources!': [
            '<(DEPTH)/chrome/common/child_process_logging_posix.cc',
            'src/api/menu/menu_delegate.cc',
            'src/api/menu/menu_delegate.h',
          ],
          'sources': [
            #'<(DEPTH)/chrome/browser/ui/cocoa/web_contents_modal_dialog_manager_cocoa.mm',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_bridge.h',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_bridge.mm',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_cocoa.h',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_cocoa.mm',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_controller.h',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_controller.mm',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_item.h',
            '<(DEPTH)/chrome/browser/ui/cocoa/media_picker/desktop_media_picker_item.mm',
            'src/browser/shell_web_contents_modal_dialog_manager.cc',
          ],
          'dependencies': [
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad',
            '<(DEPTH)/components/components.gyp:crash_component',
            '<(DEPTH)/third_party/google_toolbox_for_mac/google_toolbox_for_mac.gyp:google_toolbox_for_mac',
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/AddressBook.framework',
            ],
          },
        }],
        ['OS=="win"', {
          'sources': [
            'src/browser/color_chooser_dialog.cc',
            'src/browser/color_chooser_dialog.h',
            'src/browser/printing_handler.cc',
            'src/browser/printing_handler.h',
            'src/browser/shell_content_utility_client.cc',
            'src/browser/shell_content_utility_client.h',
            '<(DEPTH)/chrome/browser/printing/pdf_to_emf_converter.cc',
            '<(DEPTH)/chrome/utility/cloud_print/bitmap_image.cc',
            '<(DEPTH)/chrome/utility/cloud_print/bitmap_image.h',
            '<(DEPTH)/chrome/utility/cloud_print/pwg_encoder.cc',
            '<(DEPTH)/chrome/utility/cloud_print/pwg_encoder.h',
          ],
          'resource_include_dirs': [
            '<(SHARED_INTERMEDIATE_DIR)/webkit',
          ],
          'dependencies': [
            '<(DEPTH)/ui/views/controls/webview/webview.gyp:webview',
            '<(DEPTH)/ui/views/views.gyp:views',
            #'<(DEPTH)/webkit/webkit_resources.gyp:webkit_strings',
          ],
          'configurations': {
            'Debug_Base': {
              'msvs_settings': {
                'VCLinkerTool': {
                  'LinkIncremental': '<(msvs_large_module_debug_link_mode)',
                },
              },
            },
          },
          'msvs_disabled_warnings': [ 4267, 4800, 4819, 4189, 4245, 4324 ],
        }],  # OS=="win"
      ],
    },
    {
      'target_name': 'nw_base',
      'type': '<(component)',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/third_party/zlib/google/zip.gyp:zip',
        '<(DEPTH)/url/url.gyp:url_lib',
        'commit_id',
      ],
      'include_dirs': [
      ],
      'defines': ['NW_IMPLEMENTATION'],
      'sources': [
        'src/nw_base.cc',
        'src/nw_base.h',
        'src/nw_package.cc',
        'src/nw_package.h',
        'src/common/shell_switches.cc',
        'src/common/shell_switches.h',
      ],
    },
    {
      'target_name': 'nw_content',
      'type': 'static_library',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        #'<(DEPTH)/third_party/node/node.gyp:node',
        '<(DEPTH)/third_party/WebKit/public/blink.gyp:blink',
        '<(DEPTH)/third_party/WebKit/Source/wtf/wtf.gyp:wtf',
        '<(DEPTH)/third_party/zlib/zlib.gyp:minizip',
        '<(DEPTH)/skia/skia.gyp:skia',
        'nw_base',
        '<(DEPTH)/content/nw/src/api/api.gyp:nw_api',
        '<(DEPTH)/content/nw/src/api/api_registration.gyp:nw_api_registration',
        '<(DEPTH)/extensions/browser/api/api_registration.gyp:extensions_api_registration',
        '<(DEPTH)/components/components.gyp:policy',
        '<(DEPTH)/third_party/protobuf/protobuf.gyp:protobuf_lite',
        '<(DEPTH)/third_party/webrtc/modules/modules.gyp:desktop_capture',
      ],
      'include_dirs': [
        '<(DEPTH)/third_party',
        '<(DEPTH)/third_party/mojo/src',
        '<(SHARED_INTERMEDIATE_DIR)/blink',
        '<(SHARED_INTERMEDIATE_DIR)/blink/bindings/core/v8/',
      ],
      'defines!': ['CONTENT_IMPLEMENTATION'],
      'msvs_disabled_warnings': [ 4267, 4800, 4189 ],
      'sources': [
        'src/api/nw_app_api.cc',
        'src/api/nw_app_api.h',
        'src/api/nw_clipboard_api.cc',
        'src/api/nw_clipboard_api.h',
        'src/api/nw_window_api.cc',
        'src/api/nw_window_api.h',
        'src/api/nw_menu_api.cc',
        'src/api/nw_menu_api.h',
        'src/api/nw_object_api.cc',
        'src/api/nw_object_api.h',
        'src/api/nw_shell_api.cc',
        'src/api/nw_shell_api.h',
        'src/api/nw_screen_api.cc',
        'src/api/nw_screen_api.h',
        'src/api/nw_shortcut_api.cc',
        'src/api/nw_shortcut_api.h',
        'src/api/object_manager.cc',
        'src/api/object_manager.h',
        'src/api/object_manager_factory.cc',
        'src/api/object_manager_factory.h',
        'src/api/base/base.cc',
        'src/api/base/base.h',
        'src/api/menu/menu.cc',
        'src/api/menu/menu.h',
        'src/api/menuitem/menuitem.cc',
        'src/api/menuitem/menuitem.h',
        'src/api/shell/shell.cc',
        'src/api/shell/shell.h',
        'src/api/tray/tray.cc',
        'src/api/tray/tray.h',
        'src/nw_content.cc',
        'src/nw_content.h',
        'src/nw_custom_bindings.cc',
        'src/nw_custom_bindings.h',
        'src/policy_cert_verifier.cc',
        'src/policy_cert_verifier.h',
        'src/nw_content_verifier_delegate.cc',
        'src/nw_content_verifier_delegate.h',
      ],
      'conditions': [
        ['OS=="win" or OS=="linux"', {
          'sources': [
            'src/api/tray/tray_aura.cc',
            'src/api/menu/menu_delegate.cc',
            'src/api/menu/menu_delegate.h',
            'src/api/menu/menu_views.cc',
            'src/api/menu/menu_views.h',
            'src/browser/menubar_view.cc',
            'src/browser/menubar_view.h',
            'src/api/menuitem/menuitem_views.cc',
            'src/browser/browser_view_layout.cc',
            'src/browser/browser_view_layout.h',
            'src/browser/menubar_controller.cc',
            'src/browser/menubar_controller.h',
          ],
        }],
        ['OS=="mac"', {
          'sources': [
            'src/api/nw_window_api_mac.mm',
            'src/api/nw_menu_api_mac.mm',
            'src/api/menuitem/menuitem_mac.mm',
            'src/api/menu/menu_mac.mm',
            'src/api/menu/menu_delegate_mac.h',
            'src/api/menu/menu_delegate_mac.mm',
            'src/api/menuitem/menuitem_delegate_mac.h',
            'src/api/menuitem/menuitem_delegate_mac.mm',
            'src/api/tray/tray_mac.mm',
            'src/nw_content_mac.h',
            'src/nw_content_mac.mm',
          ],
        }],
      ],
    },
    {
      'target_name': 'about_credits_nw',
      'type': 'none',
      'actions': [
        {
          'variables': {
            'generator_path': '../../tools/licenses.py',
            'about_credits_file': '<(PRODUCT_DIR)/credits.html',
          },
          'action_name': 'generate_about_credits',
          'inputs': [
            # TODO(phajdan.jr): make licenses.py print inputs too.
            '<(generator_path)',
          ],
          'outputs': [
            '<(about_credits_file)',
          ],
          'hard_dependency': 1,
          'action': ['python',
                     '<(generator_path)',
                     'credits',
                     '<(about_credits_file)',
          ],
          'message': 'Generating NW credits html.',
        },
      ],
    },
    {
      # We build a minimal set of resources so WebKit in nw has
      # access to necessary resources.
      'target_name': 'nw_pak',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/content/browser/devtools/devtools_resources.gyp:devtools_resources',
        '<(DEPTH)/ui/resources/ui_resources.gyp:ui_resources',
        '<(DEPTH)/net/net.gyp:net_resources',
        '<(DEPTH)/third_party/WebKit/public/blink_resources.gyp:blink_resources',
        '<(DEPTH)/ui/strings/ui_strings.gyp:ui_strings',
        '<(DEPTH)/content/app/resources/content_resources.gyp:content_resources',
      ],
      'variables': {
        'repack_path': '<(DEPTH)/tools/grit/grit/format/repack.py',
      },
      'actions': [
        {
          'action_name': 'repack_nw_pack',
          'variables': {
            'pak_inputs': [
              '<(SHARED_INTERMEDIATE_DIR)/chrome/generated_resources_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/content_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/nw_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/nw_components.pak',
              '<(SHARED_INTERMEDIATE_DIR)/net/net_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/ui_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/webui_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/app_locale_settings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/components/strings/components_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/ui_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/blink/devtools_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources/blink_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/resources/content_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/strings/content_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/extensions/extensions_browser_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/extensions/extensions_renderer_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/extensions/extensions_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/extensions/strings/extensions_strings_en-US.pak',
            ],
          },
          'inputs': [
            '<(repack_path)',
            '<@(pak_inputs)',
          ],
          'outputs': [
            '<(PRODUCT_DIR)/nw.pak',
          ],
          'action': ['python', '<(repack_path)', '<@(_outputs)',
                     '<@(pak_inputs)'],
        },
      ],
    },
    {
      'target_name': 'nw_strip_symbol',
      'type': 'none',
      'conditions': [
       ['OS=="win"', {
          'actions': [
            {
              'action_name': 'dump_symbol_and_strip',
              'inputs': [
                '<(DEPTH)/content/nw/tools/dump_win_syms.py',
                '<(PRODUCT_DIR)/nw.exe',
                '<(PRODUCT_DIR)/nw.dll',
                '<(PRODUCT_DIR)/node.dll',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/nw.sym.7z',
              ],
              'action': ['python',
                         '<(DEPTH)/content/nw/tools/dump_win_syms.py',
                         '<(PRODUCT_DIR)/nw.exe',
                         '<(PRODUCT_DIR)/nw.dll',
                         '<(PRODUCT_DIR)/node.dll',
                         '<(PRODUCT_DIR)/nw.sym.7z',
                         ],
              'message': 'Dumping breakpad symbols to <(_outputs)',
              'process_outputs_as_sources': 1,
            },
          ],
          'dependencies': [
            'nwjs',
          ],
        }],
        ['OS=="linux"', {
          'variables': {
            'linux_strip_binary': 1,
          },
          'actions': [
            {
              'action_name': 'dump_symbol_and_strip',
              'inputs': [
                '<(DEPTH)/content/nw/tools/dump_app_syms',
                '<(PRODUCT_DIR)/dump_syms',
                '<(PRODUCT_DIR)/nw',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/nw.breakpad.<(target_arch)',
              ],
              'action': ['<(DEPTH)/content/nw/tools/dump_app_syms',
                         '<(PRODUCT_DIR)/dump_syms',
                         '<(linux_strip_binary)',
                         '<(PRODUCT_DIR)/nw',
                         '<@(_outputs)'],
              'message': 'Dumping breakpad symbols to <(_outputs)',
              'process_outputs_as_sources': 1,
            },
            {
              'action_name': 'dump_symbol_and_strip_2',
              'inputs': [
                '<(DEPTH)/content/nw/tools/dump_app_syms',
                '<(PRODUCT_DIR)/dump_syms',
                '<(PRODUCT_DIR)/lib/libnw.so',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/nw.so.breakpad.<(target_arch)',
              ],
              'action': ['<(DEPTH)/content/nw/tools/dump_app_syms',
                         '<(PRODUCT_DIR)/dump_syms',
                         '<(linux_strip_binary)',
                         '<(PRODUCT_DIR)/lib/libnw.so',
                         '<@(_outputs)'],
              'message': 'Dumping breakpad symbols to <(_outputs)',
              'process_outputs_as_sources': 1,
            },
            {
              'action_name': 'dump_symbol_and_strip_3',
              'inputs': [
                '<(DEPTH)/content/nw/tools/dump_app_syms',
                '<(PRODUCT_DIR)/dump_syms',
                '<(PRODUCT_DIR)/lib/libnode.so',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/node.so.breakpad.<(target_arch)',
              ],
              'action': ['<(DEPTH)/content/nw/tools/dump_app_syms',
                         '<(PRODUCT_DIR)/dump_syms',
                         '<(linux_strip_binary)',
                         '<(PRODUCT_DIR)/lib/libnode.so',
                         '<@(_outputs)'],
              'message': 'Dumping breakpad symbols to <(_outputs)',
              'process_outputs_as_sources': 1,
            },
          ],
          'dependencies': [
            'nwjs',
            '../breakpad/breakpad.gyp:dump_syms',
          ],
        }],
        ['OS=="linux" and disable_nacl==0', {
          'variables': {
            'linux_strip_binary': 1,
          },
          'actions': [
            {
              'action_name': 'dump_symbol_and_strip_4',
              'inputs': [
                '<(DEPTH)/content/nw/tools/dump_app_syms',
                '<(PRODUCT_DIR)/dump_syms',
                '<(PRODUCT_DIR)/nacl_helper',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/nacl_helper.breakpad.<(target_arch)',
              ],
              'action': ['<(DEPTH)/content/nw/tools/dump_app_syms',
                         '<(PRODUCT_DIR)/dump_syms',
                         '<(linux_strip_binary)',
                         '<(PRODUCT_DIR)/nacl_helper',
                         '<@(_outputs)'],
              'message': 'Dumping breakpad symbols to <(_outputs)',
              'process_outputs_as_sources': 1,
            },
          ],
        }],
      ],
    },
    {
      'target_name': 'strip_binaries',
      'type': 'none',
      'conditions': [
        ['OS=="linux"', {
          'actions': [
            {
              'action_name': 'strip_nw_binaries',
              'inputs': [
                '<(PRODUCT_DIR)/chromedriver',
                '<(PRODUCT_DIR)/nwjc',
                '<(PRODUCT_DIR)/payload',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/strip_binaries.stamp',
              ],
              'action': ['strip',
                         '<@(_inputs)'],
              'message': 'Stripping release binaries',
            },
          ],
          'dependencies': [
             '<(DEPTH)/chrome/chrome.gyp:chromedriver',
          ],
        }],
      ],
    },
    {
      'target_name': 'nwjs',
      'type': 'none',
      'dependencies': [
         '<(DEPTH)/chrome/chrome.gyp:chrome',
         '<(DEPTH)/third_party/node/node.gyp:node',
         '<(DEPTH)/v8/tools/gyp/v8.gyp:nwjc',
         'payload',
      ],
      'conditions': [
        ['disable_nacl==0', {
          'dependencies': [
            '<(DEPTH)/components/nacl.gyp:nacl',
          ]
        }],
        [ 'OS=="mac"', {
          'copies': [
            {
              'destination': '<(PRODUCT_DIR)/<(mac_product_name).app/Contents/Versions/<(version_full)/<(mac_product_name) Framework.framework/',
              'files': [
                '<(PRODUCT_DIR)/libnode.dylib',
              ],
            },
          ],
        }],
      ],
    },
    {
      'target_name': 'dist',
      'type': 'none',
      'variables': {
        'conditions': [
          ['nwjs_sdk==1', {
            'package_mode': 'sdk',
            'icudat_path': '<(DEPTH)/third_party/icu/source/data/in/icudtl.dat',
          }, {
            'package_mode': 'nosdk',
            'icudat_path': '<(DEPTH)/third_party/icu/source/data/in/icudtl.dat',
          }],
          ['disable_nacl==0 and nwjs_sdk==0', {
            'package_mode': 'nacl',
          }],
        ],  # conditions
      },  # variables
      'actions': [
        {
          'action_name': 'package_nw_binaries',
          'variables':{
            'package_script': '<(DEPTH)/content/nw/tools/package_binaries.py',
          },
          'inputs': [
            '<(package_script)',
          ],
          'outputs':[
            '<(PRODUCT_DIR)/new_package.re',
          ],
          'action': ['python', '<(package_script)', '-p', '<(PRODUCT_DIR)',
                     '-a', '<(target_arch)', '-m', '<(package_mode)',
                     '-i', '<(icudat_path)'],
        },
      ],
      'conditions': [
        ['nwjs_sdk==1', {
          'dependencies': [
            '<(DEPTH)/chrome/chrome.gyp:chromedriver',
          ],
        }],
        ['OS == "linux"', {
          'dependencies': [
            'strip_binaries',
          ],
        }],
      ],
      'dependencies': [
        'nw_strip_symbol',
        'about_credits_nw',
      ],
    },
    {
      'target_name': 'payload',
      'type': 'executable',
      'sources': [
        'tools/payload.cc',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/extensions/extensions.gyp:extensions_browser',
      ],
      'conditions': [
        ['OS=="win" and win_use_allocator_shim==1', {
          'dependencies': [
            '<(DEPTH)/base/allocator/allocator.gyp:allocator',
          ],
        }],
      ],
    },
    {
      'target_name': 'test',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/chrome/chrome.gyp:chromedriver',
      ],
      'actions': [
        {
          'action_name': 'run_tests',
          'variables':{
            'test_script': '<(DEPTH)/content/nw/test/test.py',
          },
          'inputs': [
            '<(test_script)',
          ],
          'outputs':[
            '<(PRODUCT_DIR)/run_tests.re',
          ],
          'action': ['python', '<(test_script)', '-d', '<(PRODUCT_DIR)',
                     '-t', '80', 'remoting'],
        },
      ],
    },
    {
      'target_name': 'nwlegacy',
      'type': 'executable',
      'mac_bundle': 1,
      'defines!': ['CONTENT_IMPLEMENTATION'],
      'variables': {
        'chromium_code': 1,
      },
      'dependencies': [
        'nw_lib',
        'nw_pak',
        '<(DEPTH)/pdf/pdf.gyp:pdf',
      ],
      'include_dirs': [
        '<(DEPTH)',
      ],
      'sources': [
        '../app/startup_helper_win.cc',
        'src/shell_main.cc',
      ],
      'mac_bundle_resources': [
        'src/mac/nw.icns',
        'src/mac/app-Info.plist',
      ],
      # TODO(mark): Come up with a fancier way to do this.  It should only
      # be necessary to list app-Info.plist once, not the three times it is
      # listed here.
      'mac_bundle_resources!': [
        'src/mac/app-Info.plist',
      ],
      'xcode_settings': {
        'INFOPLIST_FILE': 'nw/src/mac/app-Info.plist',
      },
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '2',  # Set /SUBSYSTEM:WINDOWS
        },
      'VCManifestTool': {
          'AdditionalManifestFiles': [
            '$(ProjectDir)\\nw\\src\\nw.exe.manifest',
          ],
        },
      },
      'conditions': [
        ['OS=="win" and win_use_allocator_shim==1', {
          'dependencies': [
            '<(DEPTH)/base/allocator/allocator.gyp:allocator',
          ],
        }],
        ['OS=="win"', {
          'dependencies': [
            '<(DEPTH)/ui/resources/ui_resources.gyp:ui_resources',
          ],
          'sources': [
            'src/shell.rc',
            '<(SHARED_INTERMEDIATE_DIR)/ui/resources/ui_unscaled_resources.rc',
          ],
          'configurations': {
            'Debug_Base': {
              'msvs_settings': {
                'VCLinkerTool': {
                  'LinkIncremental': '<(msvs_large_module_debug_link_mode)',
                },
              },
            },
          },
        }],  # OS=="win"
        ['OS == "linux"', {
          'ldflags': [
            '-Wl,--whole-archive', 'obj/third_party/node/libnode.a', '-Wl,--no-whole-archive' ],
        }],
        ['OS == "win"', {
          'dependencies': [
            '<(DEPTH)/sandbox/sandbox.gyp:sandbox',
          ],
        }],  # OS=="win" or (toolkit_uses_gtk == 1 and selinux == 0)
        ['OS=="mac"', {
          'product_name': '<(nw_product_name)',
          'dependencies!': [
            'nw_lib',
          ],
          'dependencies': [
            'nw_framework',
            'nw_helper_app',
          ],
          'copies': [
            {
              'destination': '<(PRODUCT_DIR)/<(nw_product_name).app/Contents/Frameworks',
              'files': [
                '<(PRODUCT_DIR)/<(nw_product_name) Helper.app',
                '<(PRODUCT_DIR)/crash_inspector',
              ],
            },
          ],
          'postbuilds': [
            {
              'postbuild_name': 'Copy <(nw_product_name) Framework.framework',
              'action': [
                '<(DEPTH)/build/mac/copy_framework_unversioned.sh',
                '${BUILT_PRODUCTS_DIR}/<(nw_product_name) Framework.framework',
                '${BUILT_PRODUCTS_DIR}/${CONTENTS_FOLDER_PATH}/Frameworks',
              ],
            },
            {
              'postbuild_name': 'Fix Framework Link',
              'action': [
                'install_name_tool',
                '-change',
                '/Library/Frameworks/<(nw_product_name) Framework.framework/Versions/A/<(nw_product_name) Framework',
                '@executable_path/../Frameworks/<(nw_product_name) Framework.framework/<(nw_product_name) Framework',
                '${BUILT_PRODUCTS_DIR}/${EXECUTABLE_PATH}'
              ],
            },
            {
              # Modify the Info.plist as needed.
              'postbuild_name': 'Tweak Info.plist',
              'action': ['<(DEPTH)/build/mac/tweak_info_plist.py',
                         '--breakpad=0',
                         '--keystone=0'],
            },
            {
              # This postbuid step is responsible for creating the following
              # helpers:
              #
              # Content Shell Helper EH.app and Content Shell Helper NP.app are
              # created from Content Shell Helper.app.
              #
              # The EH helper is marked for an executable heap. The NP helper
              # is marked for no PIE (ASLR).
              'postbuild_name': 'Make More Helpers',
              'action': [
                '<(DEPTH)/build/mac/make_more_helpers.sh',
                'Frameworks',
                '<(nw_product_name)',
              ],
            },
            {
              # Make sure there isn't any Objective-C in the shell's
              # executable.
              'postbuild_name': 'Verify No Objective-C',
              'action': [
                '<(DEPTH)/build/mac/verify_no_objc.sh',
              ],
            },
          ],
        }],  # OS=="mac"
      ],
    }, # target nw
    {
      'target_name': 'copy_scripts',
      'type': 'none',
      'copies':
      [
          {
              'destination': '<(nw_gen_path)',
              'files': [ '<(DEPTH)/content/nw/tools/<(nw_id_script_base)' ],
          },
      ],
    },
    {
        'target_name': 'commit_id',
        'type': 'none',
        'dependencies': [ 'copy_scripts', ],
        'actions':
        [
            {
                'action_name': 'Generate NW Commit ID Header',
                'message': 'Generating NW Commit ID',
                # reference the git index as an input, so we rebuild on changes to the index
                'inputs': [ 
                      '<(nw_id_script)', 
                      '<(DEPTH)/content/nw/.git/index',
                      '<(DEPTH)/.git/index',
                      '<(DEPTH)/v8/.git/index',
                      '<(DEPTH)/third_party/node/.git/index'],
                'outputs': [ '<(nw_id_header)' ],
                'msvs_cygwin_shell': 0,
                'action':
                [
                    'python', '<(nw_id_script)', 'gen', '<(DEPTH)', '<(nw_id_header)'
                ],
            },
        ],
        'direct_dependent_settings':
        {
            'include_dirs':
            [
                '<(nw_gen_path)',
            ],
        },
    }
  ],
  'conditions': [
    ['OS=="mac"', {
      'targets': [
        {
          'target_name': 'nw_framework',
          'type': 'shared_library',
          'product_name': '<(nw_product_name) Framework',
          'mac_bundle': 1,
          'mac_bundle_resources': [
            'src/mac/English.lproj/HttpAuth.xib',
            '<(PRODUCT_DIR)/icudtl.dat',
            '<(PRODUCT_DIR)/nw.pak',
            '<(PRODUCT_DIR)/locales/en-US.pak',
            '<(PRODUCT_DIR)/locales/am.pak',
            '<(PRODUCT_DIR)/locales/ar.pak',
            '<(PRODUCT_DIR)/locales/bg.pak',
            '<(PRODUCT_DIR)/locales/bn.pak',
            '<(PRODUCT_DIR)/locales/ca.pak',
            '<(PRODUCT_DIR)/locales/cs.pak',
            '<(PRODUCT_DIR)/locales/da.pak',
            '<(PRODUCT_DIR)/locales/de.pak',
            '<(PRODUCT_DIR)/locales/el.pak',
            '<(PRODUCT_DIR)/locales/en-GB.pak',
            '<(PRODUCT_DIR)/locales/es.pak',
            '<(PRODUCT_DIR)/locales/es-419.pak',
            '<(PRODUCT_DIR)/locales/et.pak',
            '<(PRODUCT_DIR)/locales/fa.pak',
            '<(PRODUCT_DIR)/locales/fi.pak',
            '<(PRODUCT_DIR)/locales/fil.pak',
            '<(PRODUCT_DIR)/locales/fr.pak',
            '<(PRODUCT_DIR)/locales/gu.pak',
            '<(PRODUCT_DIR)/locales/hi.pak',
            '<(PRODUCT_DIR)/locales/hr.pak',
            '<(PRODUCT_DIR)/locales/hu.pak',
            '<(PRODUCT_DIR)/locales/id.pak',
            '<(PRODUCT_DIR)/locales/it.pak',
            '<(PRODUCT_DIR)/locales/iw.pak',
            '<(PRODUCT_DIR)/locales/ja.pak',
            '<(PRODUCT_DIR)/locales/kn.pak',
            '<(PRODUCT_DIR)/locales/ko.pak',
            '<(PRODUCT_DIR)/locales/lt.pak',
            '<(PRODUCT_DIR)/locales/lv.pak',
            '<(PRODUCT_DIR)/locales/ml.pak',
            '<(PRODUCT_DIR)/locales/mr.pak',
            '<(PRODUCT_DIR)/locales/ms.pak',
            '<(PRODUCT_DIR)/locales/nl.pak',
            '<(PRODUCT_DIR)/locales/no.pak',
            '<(PRODUCT_DIR)/locales/pl.pak',
            '<(PRODUCT_DIR)/locales/pt-BR.pak',
            '<(PRODUCT_DIR)/locales/pt-PT.pak',
            '<(PRODUCT_DIR)/locales/ro.pak',
            '<(PRODUCT_DIR)/locales/ru.pak',
            '<(PRODUCT_DIR)/locales/sk.pak',
            '<(PRODUCT_DIR)/locales/sl.pak',
            '<(PRODUCT_DIR)/locales/sr.pak',
            '<(PRODUCT_DIR)/locales/sv.pak',
            '<(PRODUCT_DIR)/locales/sw.pak',
            '<(PRODUCT_DIR)/locales/ta.pak',
            '<(PRODUCT_DIR)/locales/te.pak',
            '<(PRODUCT_DIR)/locales/th.pak',
            '<(PRODUCT_DIR)/locales/tr.pak',
            '<(PRODUCT_DIR)/locales/uk.pak',
            '<(PRODUCT_DIR)/locales/vi.pak',
            '<(PRODUCT_DIR)/locales/zh-CN.pak',
            '<(PRODUCT_DIR)/locales/zh-TW.pak',
          ],
          'dependencies': [
            'nw_lib',
          ],
          'include_dirs': [
            '../..',
          ],
          'sources': [
            'src/shell_content_main.cc',
            'src/shell_content_main.h',
          ],
          'xcode_settings': { 'OTHER_LDFLAGS': [ '-Wl,-force_load,libnode.a' ], },
          'link_settings': {
            'xcode_settings': {
              'OTHER_LDFLAGS': [
                '-framework Quartz',
              ],
            },
          },
          'copies': [
            {
              # Copy FFmpeg binaries for audio/video support.
              'destination': '<(PRODUCT_DIR)/$(CONTENTS_FOLDER_PATH)/Libraries',
              'files': [
                '<(PRODUCT_DIR)/ffmpegsumo.so',
              ],
            },
          ],
        },  # target nw_framework
        {
          'target_name': 'nw_helper_app',
          'type': 'executable',
          'variables': { 'enable_wexit_time_destructors': 1, },
          'product_name': '<(nw_product_name) Helper',
          'mac_bundle': 1,
          'dependencies': [
            'nw_framework',
          ],
          'sources': [
            'src/shell_main.cc',
            'src/mac/helper-Info.plist',
          ],
          # TODO(mark): Come up with a fancier way to do this.  It should only
          # be necessary to list helper-Info.plist once, not the three times it
          # is listed here.
          'mac_bundle_resources!': [
            'src/mac/helper-Info.plist',
          ],
          # TODO(mark): For now, don't put any resources into this app.  Its
          # resources directory will be a symbolic link to the browser app's
          # resources directory.
          'mac_bundle_resources/': [
            ['exclude', '.*'],
          ],
          'xcode_settings': {
            'INFOPLIST_FILE': 'nw/src/mac/helper-Info.plist',
          },
          'postbuilds': [
            {
              # The framework defines its load-time path
              # (DYLIB_INSTALL_NAME_BASE) relative to the main executable
              # (chrome).  A different relative path needs to be used in
              # nw_helper_app.
              'postbuild_name': 'Fix Framework Link',
              'action': [
                'install_name_tool',
                '-change',
                '/Library/Frameworks/<(nw_product_name) Framework.framework/Versions/A/<(nw_product_name) Framework',
                '@executable_path/../../../../Frameworks/<(nw_product_name) Framework.framework/<(nw_product_name) Framework',
                '${BUILT_PRODUCTS_DIR}/${EXECUTABLE_PATH}'
              ],
            },
            {
              # Modify the Info.plist as needed.  The script explains why this
              # is needed.  This is also done in the chrome and chrome_dll
              # targets.  In this case, --breakpad=0, --keystone=0, and --svn=0
              # are used because Breakpad, Keystone, and Subversion keys are
              # never placed into the helper.
              'postbuild_name': 'Tweak Info.plist',
              'action': ['<(DEPTH)/build/mac/tweak_info_plist.py',
                         '--breakpad=0',
                         '--keystone=0'],
            },
            {
              # Make sure there isn't any Objective-C in the helper app's
              # executable.
              'postbuild_name': 'Verify No Objective-C',
              'action': [
                '<(DEPTH)/build/mac/verify_no_objc.sh',
              ],
            },
          ],
          'conditions': [
            ['component=="shared_library"', {
              'xcode_settings': {
                'LD_RUNPATH_SEARCH_PATHS': [
                  # Get back from Content Shell.app/Contents/Frameworks/
                  #                                 Helper.app/Contents/MacOS
                  '@loader_path/../../../../../..',
                ],
              },
            }],
          ],
        },  # target nw_helper_app
      ],
    }],  # OS=="mac"
  ] # conditions
}

