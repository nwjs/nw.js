# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'nw_api_registration',
      'type': 'static_library',
      # TODO(jschuh): http://crbug.com/167187 size_t -> int
      'msvs_disabled_warnings': [ 4267 ],
      'includes': [
        '../../../../build/json_schema_bundle_registration_compile.gypi',
        'schemas.gypi',
      ],
      'dependencies': [
        'api.gyp:nw_api',
        # Different APIs include headers from these targets.
        "<(DEPTH)/content/content.gyp:content_browser",

        # Different APIs include some headers from chrome/common that in turn
        # include generated headers from these targets.
        # TODO(brettw) this should be made unnecessary if possible.
        '<(DEPTH)/components/components.gyp:component_metrics_proto',
        '<(DEPTH)/components/components.gyp:copresence_proto',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/sync/sync.gyp:sync',
        '<(DEPTH)/ui/accessibility/accessibility.gyp:ax_gen',
      ],
    },
  ],
}
