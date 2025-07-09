# Copyright (c) 2009 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

import pickle
from optparse import OptionParser
import os
import sys
from file_util import *
from git_util import git_apply_patch_file

# Cannot be loaded as a module.
if __name__ != "__main__":
  sys.stdout.write('This file cannot be loaded as a module!')
  sys.exit()

# NWJS: Define paths in accordance with NW.js source tree hierarchy, and rename
# cef_* to nw_* for better readability.
#cef_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
#cef_patch_dir = os.path.join(cef_dir, 'patch')
#src_dir = os.path.abspath(os.path.join(cef_dir, os.pardir))
nw_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
nw_patch_dir = os.path.join(nw_dir, 'patch')
src_dir = os.path.abspath(os.path.join(nw_dir, os.pardir, os.pardir))


def write_note(type, note):
  separator = '-' * 79 + '\n'
  sys.stdout.write(separator)
  sys.stdout.write('!!!! %s: %s\n' % (type, note))
  sys.stdout.write(separator)


def apply_patch_file(patch_file, patch_dir):
  ''' Apply a specific patch file in optional patch directory. '''
  patch_path = os.path.join(nw_patch_dir, 'patches', patch_file + '.patch')

  if patch_dir is None or len(patch_dir) == 0:
    patch_dir = src_dir
  else:
    if not os.path.isabs(patch_dir):
      # Apply patch relative to the Chromium 'src' directory.
      patch_dir = os.path.join(src_dir, patch_dir)
    patch_dir = os.path.abspath(patch_dir)

  result = git_apply_patch_file(patch_path, patch_dir)
  if result == 'fail':
    write_note('ERROR',
               'This patch failed to apply. Your build will not be correct.')
  return result


def apply_patch_config():
  ''' Apply patch files based on a configuration file. '''
  config_file = os.path.join(nw_patch_dir, 'patch.cfg')
  if not os.path.isfile(config_file):
    raise Exception('Patch config file %s does not exist.' % config_file)

  # Parse the configuration file.
  scope = {}
  exec(open(config_file).read(),scope)
  patches = scope["patches"]

  results = {'apply': 0, 'skip': 0, 'fail': 0}

  for patch in patches:
    patch_file = patch['name']
    dopatch = True

    if 'condition' in patch:
      # Check that the environment variable is set.
      if patch['condition'] not in os.environ:
        sys.stdout.write('\nSkipping patch file %s\n' % patch_file)
        dopatch = False

    if dopatch:
      result = apply_patch_file(patch_file, patch['path']
                                if 'path' in patch else None)
      results[result] += 1

      if 'note' in patch:
        write_note('NOTE', patch['note'])
    else:
      results['skip'] += 1

  sys.stdout.write('\n%d patches total (%d applied, %d skipped, %d failed)\n' % \
      (len(patches), results['apply'], results['skip'], results['fail']))

  if results['fail'] > 0:
    sys.stdout.write('\n')
    write_note('ERROR',
               '%d patches failed to apply. Your build will not be correct.' %
               results['fail'])
    raise Exception(
        '%d patches failed to apply. Your build will not be correct.' %
        results['fail'])


# Parse command-line options.
disc = """
This utility applies patch files.
"""

parser = OptionParser(description=disc)
parser.add_option(
    '--patch-file', dest='patchfile', metavar='FILE', help='patch source file')
parser.add_option(
    '--patch-dir',
    dest='patchdir',
    metavar='DIR',
    help='patch target directory')
# NWJS: Path of config file has been hard-coded in this script, so this option
# is for compatibility purpose only, and it will be removed once it's no longer
# used by the hook in //src/DEPS.
parser.add_option('--patch-config', help='DEPRECATED: this is a no-op')
(options, args) = parser.parse_args()

if not options.patchfile is None:
  apply_patch_file(options.patchfile, options.patchdir)
else:
  apply_patch_config()
