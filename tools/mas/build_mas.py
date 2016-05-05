#!/usr/bin/env python

import argparse
import ConfigParser
import shutil
import os
import fnmatch
import plistlib
import tempfile
from datetime import datetime
import sys
import io

bundleid = None
verbose = False

def info(msg):
  global verbose
  if verbose:
    print '[INFO] %s' % msg

def error(msg):
  print '[ERROR] %s' % msg
  print '\nFailed.'
  sys.exit(1)

def system(cmd):
  info(cmd)
  os.system(cmd)

def check_options(config, section, required_options, msg):
  missed_options = []

  for option in required_options:
    if not config.has_option(section, option):
      missed_options.append(option)
  
  if len(missed_options) != 0:
    error(msg % (section, ', '.join(missed_options)))

def glob(pathname, pattern, returnOnFound=False):
  matches = []
  for root, dirnames, filenames in os.walk(pathname):
    for dirname in fnmatch.filter(dirnames, pattern):
      if returnOnFound:
        return os.path.join(root, dirname)
      matches.append(os.path.join(root, dirname))
    for filename in fnmatch.filter(filenames, pattern):
      if returnOnFound:
        return os.path.join(root, filename)
      matches.append(os.path.join(root, filename))
  return matches

def get_bundle_id(args):
  global bundleid
  if bundleid is None:
    plist = plistlib.readPlist(os.path.join(args.output, 'Contents/Info.plist'))
    bundleid = plist['CFBundleIdentifier']
  return bundleid

def get_from_info_plist(args, key, default=None):
  plist = plistlib.readPlist(os.path.join(args.output, 'Contents/Info.plist'))
  if key in plist:
    return plist[key]
  else:
    return default

def patch_info_plist_file(file, replaces):
  plist = plistlib.readPlist(file)
  for (key, val) in replaces:
    plist[key] = val
  plistlib.writePlist(plist, file)

def generate_infoplist_strings_file(file, items):
  with io.open(file, 'w', encoding='utf-16') as fd:
    for item in items:
      fd.write(unicode('%s = "%s";\n' % item, 'utf-8'))

def read_config(args):
  print '\nParsing config file %s' % args.config_file
  if not os.path.isfile(args.config_file):
    error('%s does not exist' % args.config_file)
  config = ConfigParser.SafeConfigParser()
  config.optionxform = str # set to str to prevent transforming into lower cases
  config.read(args.config_file)
  check_options(config, 'Sign', ['ApplicationIdentity'], 'Missed options in [%s]: %s')
  if args.pkg:
    check_options(config, 'Package', ['InstallerIdentity'], 'Missed options for --pkg in [%s]: %s')
  return config

def copy_to_output(args):
  print '\nCopying %s to %s' % (args.input, args.output)
  shutil.rmtree(args.output, ignore_errors=True)
  shutil.copytree(args.input, args.output, symlinks=True) # symblic links are required

def patch_info_plist(config, args):
  print '\nPatching Info.plist files'

  replaces = []
  for (key, val) in config.items('Info.plist'):
    replaces.append((key, val))

  file = os.path.join(args.output, 'Contents/Info.plist')
  info(file)
  patch_info_plist_file(file, replaces)

  info_plist_files = glob(os.path.join(args.output, 'Contents/Versions'), 'Info.plist')
  for file in info_plist_files:
    if 'nwjs Framework' in file:
      tmp_replaces = [('CFBundleIdentifier', '%s.framework' % get_bundle_id(args))]
    elif 'nwjs Helper' in file:
      tmp_replaces = [('CFBundleIdentifier', '%s.helper' % get_bundle_id(args))]
    else:
      error('Cannot patch unknown Info.plist %s' % file)
    info(file)
    patch_info_plist_file(file, tmp_replaces)

def patch_locales(config, args):
  print '\nPatching locales'
  locales = config.get('Resources', 'Locales').split(',')
  removed_locales = []
  generated_locales = []
  for infoplist_strings_file in glob(os.path.join(args.output, 'Contents/Resources'), 'InfoPlist.strings'):
    locale_dir = os.path.dirname(infoplist_strings_file)
    (locale, _) = os.path.splitext(os.path.basename(locale_dir))
    if locale not in locales:
      removed_locales.append(locale)
      shutil.rmtree(locale_dir)
    elif config.has_section('Locale %s' % locale):
      generated_locales.append(locale)
      generate_infoplist_strings_file(infoplist_strings_file, config.items('Locale %s' % locale))
    else:
      error('Missing [Locale %s] section' % locale)

  if len(generated_locales) > 0:
    info('Generated locales for %s' % ', '.join(generated_locales))
  if len(removed_locales) > 0:
    info('Removed locales for %s' % ', '.join(removed_locales))

  removed_paks = []
  for local_pak in glob(os.path.join(args.output, 'Contents/Versions'), 'locale.pak'):
    locale_dir = os.path.dirname(local_pak)
    (locale, _) = os.path.splitext(os.path.basename(locale_dir))
    if locale != 'en' and locale not in locales:
      removed_paks.append(locale)
      shutil.rmtree(locale_dir)

  if len(removed_paks) > 0:
    info('Removed .pak files for %s' % ', '.join(removed_locales))

def patch_icon(config, args):
  plist = plistlib.readPlist(os.path.join(args.output, 'Contents/Info.plist'))
  icon = os.path.join(os.path.dirname(args.config_file), config.get('Resources', 'Icon'))
  dest_icon = os.path.join(args.output, 'Contents/Resources/%s' % plist['CFBundleIconFile'])
  info('Copying icon from %s to %s' % (icon, dest_icon))
  shutil.copy2(icon, dest_icon)

def codesign_app(config, args):
  print '\nCodesigning'

  bundleid  = get_bundle_id(args)

  identity  = config.get('Sign', 'ApplicationIdentity')
  sandbox = True
  if config.has_option('Sign', 'Sandbox'):
    sandbox = config.getboolean('Sign', 'Sandbox')

  ## sign child frameworks and helpers
  (_, tmp_child_entitlements) = tempfile.mkstemp()
  if config.has_option('Sign', 'ChildEntitlements'):
    child   = config.get('Sign', 'ChildEntitlements')
    child_entitlements = plistlib.readPlist(child)
  else:
    child_entitlements = {
      'com.apple.security.app-sandbox'  : sandbox,
      'com.apple.security.inherit'      : True
    }

  plistlib.writePlist(child_entitlements, tmp_child_entitlements)
  info('Child entitlements: %s' % tmp_child_entitlements)
  framework = glob(args.output, 'nwjs Framework.framework', returnOnFound=True)
  system('codesign -f --verbose -s "%s" --entitlements %s --deep "%s"' % (identity, tmp_child_entitlements, framework))
  helperApp = glob(args.output, 'nwjs Helper.app', returnOnFound=True)
  system('codesign -f --verbose -s "%s" --entitlements %s --deep "%s"' % (identity, tmp_child_entitlements, helperApp))

  ## sign parent app
  (_, tmp_parent_entitlements) = tempfile.mkstemp()
  if config.has_option('Sign', 'ParentEntitlements'):
    parent    = config.get('Sign', 'ParentEntitlements')
    parent_entitlements = plistlib.readPlist(parent)
  else:
    parent_entitlements = {}
  teamid = get_from_info_plist(args, 'NWTeamID', default=None)
  if teamid is None:
    groupid = bundleid
  else:
    groupid = '%s.%s' % (teamid, bundleid)
  parent_entitlements['com.apple.security.app-sandbox'] = sandbox
  parent_entitlements['com.apple.security.application-groups'] = [groupid]
  plistlib.writePlist(parent_entitlements, tmp_parent_entitlements)

  info('Parent entitlements: %s' % tmp_parent_entitlements)
  system('codesign -f --verbose -s "%s" --entitlements %s --deep "%s"' % (identity, tmp_parent_entitlements, args.output))

def productbuild(config, args):
  print '\nRunning productbuild'
  installer_identity = config.get('Package', 'InstallerIdentity')
  if config.has_option('Package', 'InstallPath'):
    install_path = config.get('Package', 'InstallPath')
  else:
    install_path = '/Applications'
  system('productbuild --component "%s" "%s" --sign "%s" "%s"' % (args.output, install_path, installer_identity, args.pkg))

def main():
  parser = argparse.ArgumentParser(description='Signing tool for NW.js app')
  parser.add_argument('-C', '--config-file', default='build.cfg', help='config file. (default: build.cfg)')
  parser.add_argument('-I', '--input', default='nwjs.app', help='path to input app. (default: nwjs.app)')
  parser.add_argument('-O', '--output', default='nwjs_output.app', help='path to output app. (default: nwjs_output.app)')
  parser.add_argument('-S', '--skip-patching', default=False, help='run codesign without patching the app. (default: False)', action='store_true')
  parser.add_argument('-P', '--pkg', default=None, help='run productbuild to generate .pkg after codesign. (default: None)')
  parser.add_argument('-V', '--verbose', default=False, help='display detailed information. (default: False)', action='store_true')
  args = parser.parse_args()

  global verbose
  verbose = args.verbose

  # read config file
  config = read_config(args)

  # make a copy
  copy_to_output(args)

  if not args.skip_patching:
    # patch Info.plist
    patch_info_plist(config, args)

    # process resources & locales
    if config.has_section('Resources'):
      if config.has_option('Resources', 'Locales'):
        patch_locales(config, args)
      if config.has_option('Resources', 'Icon'):
        patch_icon(config, args)

  # codesign
  codesign_app(config, args)

  if args.pkg:
    productbuild(config, args)

  print '\nDone.'

if __name__ == "__main__":
  main()
