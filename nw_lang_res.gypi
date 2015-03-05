{
  'variables': {
    'repack_path': '<(DEPTH)/tools/grit/grit/format/repack.py',
    'pak_inputs': [
      '<(SHARED_INTERMEDIATE_DIR)/content/<(lang).pak',
      '<(SHARED_INTERMEDIATE_DIR)/chrome/generated_resources_<(lang).pak',
      '<(SHARED_INTERMEDIATE_DIR)/ui/strings/app_locale_settings_<(lang).pak',
      '<(SHARED_INTERMEDIATE_DIR)/components/strings/components_strings_<(lang).pak',
      '<(SHARED_INTERMEDIATE_DIR)/ui/strings/ui_strings_<(lang).pak',
      '<(SHARED_INTERMEDIATE_DIR)/content/app/strings/content_strings_<(lang).pak',
      '<(SHARED_INTERMEDIATE_DIR)/extensions/strings/extensions_strings_<(lang).pak',
    ],
  },
  'inputs': [
    '<(repack_path)',
    '<@(pak_inputs)',
  ],
  'outputs': [
    '<(PRODUCT_DIR)/locales/<(lang).pak',
  ],
  'action': ['python', '<(repack_path)', '<@(_outputs)', '<@(pak_inputs)'],
  'message': 'repacking <(lang).pak',
}
