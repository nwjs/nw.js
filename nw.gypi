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
            'icudat_path': '<(DEPTH)/third_party/icu/common/icudtl.dat',
          }, {
            'package_mode': 'nosdk',
            'icudat_path': '<(DEPTH)/third_party/icu/common/icudtl.dat',
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
                     '-t', '80', 'sanity'],
        },
      ],
    },
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
}

