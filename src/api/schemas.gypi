# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'sources': [
    '<@(schema_files)',
  ],
  'variables': {
    'schema_files': [
      'nw_app.idl',
      'nw_window.idl',
      'nw_current_window_internal.idl',
    ],
    'non_compiled_schema_files': [
    ],
    
    'chromium_code': 1,
    'cc_dir': 'content/nw/src/api',
    'root_namespace': 'extensions::nwapi::%(namespace)s',
    'impl_dir_': 'content/nw/src/api',
  },
}
