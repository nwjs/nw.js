{
  'targets': [
    {
      'target_name': 'nw_api',
      'type': 'static_library',
      'includes': [
        '../../../../build/json_schema_bundle_compile.gypi',
        '../../../../build/json_schema_compile.gypi',
        'schemas.gypi',
      ],
    },
  ]
}