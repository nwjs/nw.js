# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'sources': [
    '<@(schema_files)',
  ],
  'variables': {
    'schema_files': [
      'nw_object.idl',
      'nw_app.idl',
      'nw_auto_updater.idl',
      'nw_window.idl',
      'nw_clipboard.idl',
      'nw_menu.idl',
      'nw_screen.idl',
      'nw_shell.idl',
      'nw_shortcut.idl',
      'nw_current_window_internal.idl',
      'nw_test.idl',
    ],
    'non_compiled_schema_files': [
    ],
    'schema_dependencies': [
      '<(DEPTH)/extensions/common/api/api.gyp:extensions_api',
    ],
    'chromium_code': 1,
    'cc_dir': 'content/nw/src/api',
    'root_namespace': 'extensions::nwapi::%(namespace)s',
    'bundle_name': 'nwjs',
    'impl_dir_': 'content/nw/src/api',
  },
}
