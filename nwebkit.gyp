{
  'targets': [
    {
      'target_name': 'nwebkit',
      'type': 'shared_library',
      'dependencies': [
         '../../cef/cef.gyp:libcef',
         '../../v8/tools/gyp/v8.gyp:v8',
         '../node/node.gyp:node',
        '../libuv/uv.gyp:uv',
      ],
      'defines': [
        'USING_CEF_SHARED',
      ],
      'include_dirs': [
        '.',
        'src',
      ],
      'cflags!': ['-fvisibility=hidden'],
      'cflags_cc!': ['-fvisibility-inlines-hidden'],
      'sources': [
        'src/cef.cc',
      ],
    }
  ],
}
