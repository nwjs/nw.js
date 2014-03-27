# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'nw_product_name': 'node-webkit',
    'mac_strip_release': 1,
  },
  'target_defaults': {
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
        '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
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
        '<(DEPTH)/content/content.gyp:content_worker',
        '<(DEPTH)/content/content_resources.gyp:content_resources',
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
        '<(DEPTH)/third_party/zlib/zlib.gyp:minizip',
        '<(DEPTH)/third_party/WebKit/public/blink.gyp:blink',
        'nw_resources',
      ],
      'include_dirs': [
        '<(DEPTH)',
        '<(DEPTH)/third_party/WebKit/Source',
        '<(DEPTH)/third_party/WebKit/public/web',
        '<(DEPTH)/breakpad/src',
        '<(SHARED_INTERMEDIATE_DIR)/blink',
        '<(SHARED_INTERMEDIATE_DIR)/blink/bindings',
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
        '<(DEPTH)/chrome/browser/platform_util_common_linux.cc',
        '<(DEPTH)/chrome/browser/platform_util_linux.cc',
        '<(DEPTH)/chrome/browser/platform_util_mac.mm',
        '<(DEPTH)/chrome/browser/platform_util_win.cc',
        '<(DEPTH)/chrome/browser/platform_util.h',
        '<(DEPTH)/chrome/browser/process_singleton_linux.cc',
        '<(DEPTH)/chrome/browser/process_singleton_mac.cc',
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
        '<(DEPTH)/chrome/browser/ui/gtk/gtk_window_util.cc',
        '<(DEPTH)/chrome/browser/ui/gtk/gtk_window_util.h',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_icon_win.cc',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_icon_win.h',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_win.cc',
        '<(DEPTH)/chrome/browser/ui/views/status_icons/status_tray_win.h',
        '<(DEPTH)/chrome/common/chrome_constants.cc',
        '<(DEPTH)/chrome/common/chrome_constants.h',
        '<(DEPTH)/chrome/common/chrome_switches.cc',
        '<(DEPTH)/chrome/common/chrome_switches.h',
        '<(DEPTH)/extensions/common/draggable_region.cc',
        '<(DEPTH)/extensions/common/draggable_region.h',
        '<(DEPTH)/third_party/zlib/google/zip.cc',
        '<(DEPTH)/third_party/zlib/google/zip.h',
        '<(DEPTH)/third_party/zlib/google/zip_reader.cc',
        '<(DEPTH)/third_party/zlib/google/zip_reader.h',
        '<(DEPTH)/third_party/zlib/google/zip_internal.cc',
        '<(DEPTH)/third_party/zlib/google/zip_internal.h',
        '<(DEPTH)/components/autofill/content/renderer/page_click_listener.h',
        '<(DEPTH)/components/autofill/content/renderer/page_click_tracker.cc',
        '<(DEPTH)/components/autofill/content/renderer/page_click_tracker.h',
        '<(DEPTH)/chrome/renderer/static_v8_external_string_resource.cc',
        '<(DEPTH)/chrome/renderer/static_v8_external_string_resource.h',
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
        'src/api/window_bindings.cc',
        'src/api/window_bindings.h',
        'src/api/menu/menu.cc',
        'src/api/menu/menu.h',
        'src/api/menu/menu_delegate_win.cc',
        'src/api/menu/menu_delegate_win.h',
        'src/api/menu/menu_gtk.cc',
        'src/api/menu/menu_mac.mm',
        'src/api/menu/menu_win.cc',
        'src/api/menuitem/menuitem.cc',
        'src/api/menuitem/menuitem.h',
        'src/api/menuitem/menuitem_gtk.cc',
        'src/api/menuitem/menuitem_mac.mm',
        'src/api/menuitem/menuitem_win.cc',
        'src/api/menuitem/menuitem_delegate_mac.h',
        'src/api/menuitem/menuitem_delegate_mac.mm',
        'src/api/shell/shell.cc',
        'src/api/shell/shell.h',
        'src/api/shortcut/global_shortcut_listener.cc',
        'src/api/shortcut/global_shortcut_listener.h',
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
        'src/api/tray/tray_gtk.cc',
        'src/api/tray/tray_mac.mm',
        'src/api/tray/tray_win.cc',
        'src/api/window/window.cc',
        'src/api/window/window.h',
        'src/browser/app_controller_mac.h',
        'src/browser/app_controller_mac.mm',
        'src/browser/autofill_popup_view_gtk.cc',
        'src/browser/autofill_popup_view_gtk.h',
        'src/browser/autofill_popup_view_cocoa.h',
        'src/browser/autofill_popup_view_cocoa.mm',
        'src/browser/autofill_popup_view_bridge.h',
        'src/browser/autofill_popup_view_bridge.mm',
        'src/browser/autofill_popup_controller_impl.cc',
        'src/browser/autofill_popup_controller_impl.h',
        'src/browser/tab_autofill_manager_delegate.cc',
        'src/browser/tab_autofill_manager_delegate.h',
        'src/browser/capture_page_helper.h',
        'src/browser/capture_page_helper.cc',
        'src/browser/color_chooser_gtk.cc',
        'src/browser/color_chooser_win.cc',
        'src/browser/color_chooser_mac.mm',
        'src/browser/chrome_event_processing_window.mm',
        'src/browser/chrome_event_processing_window.h',
        'src/browser/file_select_helper.cc',
        'src/browser/file_select_helper.h',
        'src/browser/native_window.cc',
        'src/browser/native_window.h',
        'src/browser/native_window_gtk.cc',
        'src/browser/native_window_gtk.h',
        'src/browser/native_window_helper_mac.h',
        'src/browser/native_window_mac.h',
        'src/browser/native_window_mac.mm',
        'src/browser/native_window_toolbar_win.cc',
        'src/browser/native_window_toolbar_win.h',
        'src/browser/native_window_win.cc',
        'src/browser/native_window_win.h',
        'src/browser/net_disk_cache_remover.cc',
        'src/browser/net_disk_cache_remover.h',
        'src/browser/nw_form_database_service.cc',
        'src/browser/nw_form_database_service.h',
        'src/browser/popup_controller_common.cc',
        'src/browser/popup_controller_common.h',
        'src/browser/printing/print_dialog_gtk.cc',
        'src/browser/printing/print_dialog_gtk.h',
        'src/browser/printing/print_job.cc',
        'src/browser/printing/print_job.h',
        'src/browser/printing/print_job_manager.cc',
        'src/browser/printing/print_job_manager.h',
        'src/browser/printing/print_job_worker.cc',
        'src/browser/printing/print_job_worker.h',
        'src/browser/printing/print_job_worker_owner.h',
        'src/browser/printing/printing_message_filter.cc',
        'src/browser/printing/printing_message_filter.h',
        'src/browser/printing/printing_ui_web_contents_observer.cc',
        'src/browser/printing/printing_ui_web_contents_observer.h',
        'src/browser/printing/printer_query.cc',
        'src/browser/printing/printer_query.h',
        'src/browser/printing/print_view_manager.cc',
        'src/browser/printing/print_view_manager.h',
        'src/browser/shell_application_mac.h',
        'src/browser/shell_application_mac.mm',
        'src/browser/shell_devtools_delegate.cc',
        'src/browser/shell_devtools_delegate.h',
        'src/browser/shell_download_manager_delegate.cc',
        'src/browser/shell_download_manager_delegate.h',
        'src/browser/shell_download_manager_delegate_win.cc',
        'src/browser/shell_download_manager_delegate_gtk.cc',
        'src/browser/shell_download_manager_delegate_mac.mm',
        'src/browser/shell_javascript_dialog_creator.cc',
        'src/browser/shell_javascript_dialog_creator.h',
        'src/browser/shell_javascript_dialog_gtk.cc',
        'src/browser/shell_javascript_dialog_mac.mm',
        'src/browser/shell_javascript_dialog_win.cc',
        'src/browser/shell_javascript_dialog.h',
        'src/browser/shell_login_dialog_gtk.cc',
        'src/browser/shell_login_dialog_mac.mm',
        #FIXME 'src/browser/shell_login_dialog_win.cc',
        'src/browser/shell_login_dialog.cc',
        'src/browser/shell_login_dialog.h',
        'src/browser/shell_resource_dispatcher_host_delegate.cc',
        'src/browser/shell_resource_dispatcher_host_delegate.h',
        'src/browser/shell_toolbar_delegate_mac.h',
        'src/browser/shell_toolbar_delegate_mac.mm',
        'src/browser/standard_menus_mac.h',
        'src/browser/standard_menus_mac.mm',
        'src/chrome_breakpad_client.cc',
        'src/chrome_breakpad_client.h',
        'src/chrome_breakpad_client_mac.mm',
        'src/common/print_messages.cc',
        'src/common/print_messages.h',
        'src/common/shell_switches.cc',
        'src/common/shell_switches.h',
        'src/breakpad_linux_impl.h',
        'src/breakpad_mac.mm',
        'src/breakpad_mac.h',
        'src/breakpad_win.cc',
        'src/breakpad_win.h',
        'src/hard_error_handler_win.cc',
        'src/hard_error_handler_win.h',
        'src/geolocation/shell_access_token_store.cc',
        'src/geolocation/shell_access_token_store.h',
        'src/media/media_internals.cc',
        'src/media/media_internals.h',
        'src/media/media_capture_devices_dispatcher.cc',
        'src/media/media_capture_devices_dispatcher.h',
        'src/media/media_stream_devices_controller.cc',
        'src/media/media_stream_devices_controller.h',
        'src/net/app_protocol_handler.cc',
        'src/net/app_protocol_handler.h',
        'src/net/clear_on_exit_policy.h',
        'src/net/clear_on_exit_policy.cc',
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
        'src/renderer/prerenderer/prerenderer_client.cc',
        'src/renderer/prerenderer/prerenderer_client.h',
        'src/renderer/printing/print_web_view_helper.cc',
        'src/renderer/printing/print_web_view_helper.h',
        'src/renderer/printing/print_web_view_helper_linux.cc',
        'src/renderer/printing/print_web_view_helper_mac.mm',
        'src/renderer/printing/print_web_view_helper_win.cc',
        'src/renderer/nw_render_view_observer.cc',
        'src/renderer/nw_render_view_observer.h',
        'src/renderer/shell_content_renderer_client.cc',
        'src/renderer/shell_content_renderer_client.h',
        'src/renderer/shell_render_process_observer.cc',
        'src/renderer/shell_render_process_observer.h',
        'src/nw_shell.cc',
        'src/nw_shell.h',
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
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '2',  # Set /SUBSYSTEM:WINDOWS
        },
      },
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
        ['use_aura==1', {
          'dependencies': [
            '<(DEPTH)/ui/views/views.gyp:views',
            '<(DEPTH)/ui/views/views.gyp:views_test_support',
            '<(DEPTH)/ui/views/controls/webview/webview.gyp:webview',
          ],
        }],
        ['(os_posix==1 and OS != "mac" and linux_use_tcmalloc==1)', {
          'dependencies': [
            # This is needed by content/app/content_main_runner.cc
            '<(DEPTH)/base/allocator/allocator.gyp:allocator',
          ],
        }],
        ['OS=="win"', {
          'sources': [
            'src/browser/autofill_popup_base_view.cc',
            'src/browser/autofill_popup_base_view.h',
            'src/browser/autofill_popup_view_views.cc',
            'src/browser/autofill_popup_view_views.h',
          ],
          'dependencies': [
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad_handler',
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad_sender',
            '<(DEPTH)/components/components.gyp:breakpad_component',
          ],
        }],
        ['os_posix==1 and OS != "mac" and OS != "ios"', {
          'sources': [
            'src/breakpad_linux.cc',
            'src/breakpad_linux.h',
            'src/crash_handler_host_linux.cc',
            'src/crash_handler_host_linux.h',
          ],
          'dependencies': [
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad_client',
            '<(DEPTH)/components/components.gyp:breakpad_component',
          ],
        }],
        ['OS == "mac"', {
          'sources!': [
            '<(DEPTH)/chrome/common/child_process_logging_posix.cc',
          ],
          'dependencies': [
            '<(DEPTH)/breakpad/breakpad.gyp:breakpad',
            '<(DEPTH)/components/components.gyp:breakpad_component',
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/AddressBook.framework',
            ],
          },
        }],
        ['toolkit_uses_gtk == 1', {
          'dependencies': [
            # For FT_Init_FreeType and friends.
            '../build/linux/system.gyp:freetype2',
            '../build/linux/system.gyp:gtk',
            '../build/linux/system.gyp:gtkprint',
          ],
        }],
        ['OS=="win"', {
          'sources': [
            'src/browser/color_chooser_dialog.cc',
            'src/browser/color_chooser_dialog.h',
          ],
          'resource_include_dirs': [
            '<(SHARED_INTERMEDIATE_DIR)/webkit',
          ],
          'dependencies': [
            '<(DEPTH)/ui/resources/ui_resources.gyp:ui_resources',
            '<(DEPTH)/ui/views/controls/webview/webview.gyp:webview',
            '<(DEPTH)/ui/views/views.gyp:views',
            '<(DEPTH)/webkit/webkit_resources.gyp:webkit_resources',
            '<(DEPTH)/webkit/webkit_resources.gyp:webkit_strings',
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
          'msvs_disabled_warnings': [ 4800 ],
        }],  # OS=="win"
      ],
    },
    {
      'target_name': 'nw_resources',
      'type': 'none',
      'dependencies': [
        'generate_nw_resources',
        'about_credits',
      ],
      'variables': {
        'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/content',
      },
      'includes': [ '../../build/grit_target.gypi' ],
      'copies': [
        {
          'destination': '<(PRODUCT_DIR)',
          'files': [
            '<(SHARED_INTERMEDIATE_DIR)/content/nw_resources.pak',
          ],
        },
        {
          'destination': '<(PRODUCT_DIR)/locales',
          'files': [
            '<(SHARED_INTERMEDIATE_DIR)/content/en-US.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/am.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ar.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/bg.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/bn.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ca.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/cs.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/da.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/de.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/el.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/en-GB.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/es.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/es-419.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/et.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/fa.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/fi.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/fil.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/fr.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/gu.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/hi.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/hr.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/hu.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/id.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/it.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/iw.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ja.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/kn.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ko.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/lt.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/lv.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ml.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/mr.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ms.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/nl.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/no.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/pl.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/pt-BR.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/pt-PT.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ro.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ru.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/sk.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/sl.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/sr.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/sv.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/sw.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/ta.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/te.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/th.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/tr.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/uk.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/vi.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/zh-CN.pak',
            '<(SHARED_INTERMEDIATE_DIR)/content/zh-TW.pak',
          ],
        },
      ],
    },
    {
      'target_name': 'generate_nw_resources',
      'type': 'none',
      'variables': {
        'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/content',
      },
      'actions': [
        {
          'action_name': 'nw_resources',
          'variables': {
            'grit_grd_file': 'src/resources/nw_resources.grd',
          },
          'includes': [ '../../build/grit_action.gypi' ],
        },
        {
          'action_name': 'nw_strings',
          'variables': {
            'grit_grd_file': 'src/resources/nw_strings.grd',
          },
          'includes': [ '../../build/grit_action.gypi' ],
        },
      ],
    },
    {
      'target_name': 'about_credits',
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
          'message': 'Generating about:credits.',
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
        'nw_resources',
      ],
      'variables': {
        'repack_path': '<(DEPTH)/tools/grit/grit/format/repack.py',
      },
      'actions': [
        {
          'action_name': 'repack_nw_pack',
          'variables': {
            'pak_inputs': [
              '<(SHARED_INTERMEDIATE_DIR)/content/content_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/nw_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/net/net_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/app_locale_settings/app_locale_settings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/ui_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/ui_strings/ui_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/webkit/devtools_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/webkit/blink_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/webkit/webkit_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/webkit/webkit_strings_en-US.pak',
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
        ['OS=="mac"', {
          'variables': {
          },
          'actions': [
            {
              'action_name': 'dump_symbol',
              'inputs': [
                '<(DEPTH)/content/nw/tools/dump_mac_syms',
                '<(PRODUCT_DIR)/dump_syms',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/node-webkit.breakpad.tar.bz2',
              ],
              'action': ['<(DEPTH)/content/nw/tools/dump_mac_syms',
                         ],
              'message': 'Dumping breakpad symbols to <(_outputs)',
              'process_outputs_as_sources': 1,
            },
          ],
          'dependencies': [
            'nw',
            '../breakpad/breakpad.gyp:dump_syms',
          ],
        }],
       ['OS=="win"', {
          'actions': [
            {
              'action_name': 'dump_symbol_and_strip',
              'inputs': [
                '<(DEPTH)/content/nw/tools/dump_win_syms.py',
                '<(PRODUCT_DIR)/nw.exe',
              ],
              'outputs': [
                '<(PRODUCT_DIR)/nw.sym.7z',
              ],
              'action': ['python',
                         '<(DEPTH)/content/nw/tools/dump_win_syms.py',
                         '<(PRODUCT_DIR)/nw.exe',
                         '<(PRODUCT_DIR)/nw.sym',
                         ],
              'message': 'Dumping breakpad symbols to <(_outputs)',
              'process_outputs_as_sources': 1,
            },
          ],
          'dependencies': [
            'nw',
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
          ],
          'dependencies': [
            'nw',
            '../breakpad/breakpad.gyp:dump_syms',
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
#                '<(PRODUCT_DIR)/nwsnapshot',
                '<(PRODUCT_DIR)/chromedriver',
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
#             '<(DEPTH)/v8/tools/gyp/v8.gyp:nwsnapshot',
             '<(DEPTH)/chrome/chrome.gyp:chromedriver',
          ],
        }],
      ],
    },
    {
      'target_name': 'dist',
      'type': 'none',
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
          'action': ['python', '<(package_script)'],
        },
      ],
      'dependencies': [
        '<(DEPTH)/chrome/chrome.gyp:chromedriver',
        'nw_strip_symbol',
      ],
      'conditions': [
        ['OS == "linux"', {
          'dependencies': [
            'strip_binaries',
          ],
        }],
      ],
    },
    {
      'target_name': 'nw',
      'type': 'executable',
      'mac_bundle': 1,
      'defines!': ['CONTENT_IMPLEMENTATION'],
      'variables': {
        'chromium_code': 1,
      },
      'dependencies': [
        'nw_lib',
        'nw_pak',
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
          'sources': [
            'src/shell.rc',
            '<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/ui_unscaled_resources.rc',
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
        ['OS == "win" or toolkit_uses_gtk == 1', {
          'dependencies': [
            '<(DEPTH)/sandbox/sandbox.gyp:sandbox',
          ],
        }],  # OS=="win" or (toolkit_uses_gtk == 1 and selinux == 0)
        ['toolkit_uses_gtk == 1', {
          'dependencies': [
            '<(DEPTH)/build/linux/system.gyp:gtk',
          ],
        }],  # toolkit_uses_gtk
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
    },
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

