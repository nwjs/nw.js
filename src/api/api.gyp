{
  'targets': [
    {
      'target_name': 'nw_api',
      'type': 'static_library',
      'sources': [
        '<@(schema_files)',
      ],
      # TODO(jschuh): http://crbug.com/167187 size_t -> int
      'msvs_disabled_warnings': [ 4267 ],
      'includes': [
        '../../../../build/json_schema_bundle_compile.gypi',
        '../../../../build/json_schema_compile.gypi',
        'schemas.gypi',
      ],
      'dependencies': [
        '<@(schema_dependencies)',
      ],
    },
  ]
}