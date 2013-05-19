# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'nw_product_name': 'node-webkit',
  },
  'targets': [
    {
      'target_name': 'nw_lib',
      'type': 'static_library',
      'defines!': ['CONTENT_IMPLEMENTATION'],
      'variables': {
        'chromium_code': 1,
      },
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(DEPTH)/build/temp_gyp/googleurl.gyp:googleurl',
        '<(DEPTH)/components/components.gyp:autofill_renderer',
        '<(DEPTH)/content/content.gyp:content_app',
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
        '<(DEPTH)/ui/ui.gyp:ui',
        '<(DEPTH)/ui/ui.gyp:ui_resources',
        '<(DEPTH)/v8/tools/gyp/v8.gyp:v8',
        '<(DEPTH)/webkit/support/webkit_support.gyp:webkit_support',
        '<(DEPTH)/third_party/zlib/zlib.gyp:minizip',
        '<(webkit_src_dir)/Source/WebKit/chromium/WebKit.gyp:webkit',
        'nw_resources',
      ],
      'include_dirs': [
        '<(DEPTH)',
      ],
      'sources': [
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
        '<(DEPTH)/components/autofill/renderer/page_click_listener.h',
        '<(DEPTH)/components/autofill/renderer/page_click_tracker.cc',
        '<(DEPTH)/components/autofill/renderer/page_click_tracker.h',
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
        'src/api/tray/tray.cc',
        'src/api/tray/tray.h',
        'src/api/tray/tray_gtk.cc',
        'src/api/tray/tray_mac.mm',
        'src/api/tray/tray_win.cc',
        'src/api/window/window.cc',
        'src/api/window/window.h',
        'src/browser/app_controller_mac.h',
        'src/browser/app_controller_mac.mm',
        'src/browser/capture_page_helper.h',
        'src/browser/capture_page_helper.cc',
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
        'src/browser/shell_login_dialog_win.cc',
        'src/browser/shell_login_dialog.cc',
        'src/browser/shell_login_dialog.h',
        'src/browser/shell_resource_dispatcher_host_delegate.cc',
        'src/browser/shell_resource_dispatcher_host_delegate.h',
        'src/browser/shell_toolbar_delegate_mac.h',
        'src/browser/shell_toolbar_delegate_mac.mm',
        'src/browser/standard_menus_mac.h',
        'src/browser/standard_menus_mac.mm',
        'src/common/print_messages.cc',
        'src/common/print_messages.h',
        'src/common/shell_switches.cc',
        'src/common/shell_switches.h',
        'src/geolocation/shell_access_token_store.cc',
        'src/geolocation/shell_access_token_store.h',
        'src/media/media_internals.cc',
        'src/media/media_internals.h',
        'src/media/media_capture_devices_dispatcher.cc',
        'src/media/media_capture_devices_dispatcher.h',
        'src/media/media_stream_devices_controller.cc',
        'src/media/media_stream_devices_controller.h',
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
        'src/shell_main_delegate.cc',
        'src/shell_main_delegate.h',
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '2',  # Set /SUBSYSTEM:WINDOWS
        },
      },
      'conditions': [
        ['OS=="win" and win_use_allocator_shim==1', {
          'dependencies': [
            '<(DEPTH)/base/allocator/allocator.gyp:allocator',
          ],
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
          'resource_include_dirs': [
            '<(SHARED_INTERMEDIATE_DIR)/webkit',
          ],
          'dependencies': [
            '<(DEPTH)/ui/ui.gyp:ui_resources',
            '<(DEPTH)/ui/views/controls/webview/webview.gyp:webview',
            '<(DEPTH)/ui/views/views.gyp:views',
            '<(DEPTH)/webkit/support/webkit_support.gyp:webkit_resources',
            '<(DEPTH)/webkit/support/webkit_support.gyp:webkit_strings',
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
      ],
      'variables': {
        'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/content',
      },
      'includes': [ '../../build/grit_target.gypi' ],
      'copies': [
        {
          'destination': '<(PRODUCT_DIR)',
          'files': [
            '<(SHARED_INTERMEDIATE_DIR)/content/nw_resources.pak'
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
      ],
    },
    {
      # We build a minimal set of resources so WebKit in nw has
      # access to necessary resources.
      'target_name': 'nw_pak',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/content/browser/devtools/devtools_resources.gyp:devtools_resources',
        '<(DEPTH)/ui/ui.gyp:ui_resources',
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
              '<(SHARED_INTERMEDIATE_DIR)/webkit/webkit_chromium_resources.pak',
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
      'target_name': 'nw_package_bin',
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
          'AdditionalOptions': [
            "\"/manifestdependency:type='win32' "
                "name='Microsoft.Windows.Common-Controls' "
                "version='6.0.0.0' "
                "processorArchitecture='*' "
                "publicKeyToken='6595b64144ccf1df' language='*'\"",
          ],
          'SubSystem': '2',  # Set /SUBSYSTEM:WINDOWS
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
            '<(SHARED_INTERMEDIATE_DIR)/webkit/webkit_unscaled_resources.rc',
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
        ['OS == "win" or (toolkit_uses_gtk == 1 and selinux == 0)', {
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
            '<(PRODUCT_DIR)/nw.pak'
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
  ]
}
