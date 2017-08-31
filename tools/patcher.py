# Copyright (c) 2009 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

import pickle
from optparse import OptionParser
import os
import sys
from file_util import *
from patch_util import *


# Cannot be loaded as a module.
if __name__ != "__main__":
  sys.stdout.write('This file cannot be loaded as a module!')
  sys.exit()


def normalize_dir(dir):
  ''' Normalize the directory value. '''
  dir = dir.replace('\\', '/')
  if dir[-1] != '/':
    dir += '/'
  return dir

def patch_file(patch_file, patch_dir):
  ''' Apply a single patch file in a single directory. '''
  if not os.path.isfile(patch_file):
    raise Exception('Patch file %s does not exist.' % patch_file)

  sys.stdout.write('Reading patch file %s\n' % patch_file)
  patchObj = from_file(patch_file)
  patchObj.apply(normalize_dir(patch_dir))

def patch_config(config_file):
  ''' Apply patch files based on a configuration file. '''
  # Normalize the patch directory value.
  patchdir = normalize_dir(os.path.dirname(os.path.abspath(config_file)))

  if not os.path.isfile(config_file):
    raise Exception('Patch config file %s does not exist.' % config_file)

  # Parse the configuration file.
  scope = {}
  execfile(config_file, scope)
  patches = scope["patches"]

  for patch in patches:
    file = patchdir+'patches/'+patch['name']+'.patch'
    dopatch = True

    if 'condition' in patch:
      # Check that the environment variable is set.
      if patch['condition'] not in os.environ:
        sys.stdout.write('Skipping patch file %s\n' % file)
        dopatch = False

    if dopatch:
      patch_file(file, patch['path'])
      if 'note' in patch:
        separator = '-' * 79 + '\n'
        sys.stdout.write(separator)
        sys.stdout.write('NOTE: %s\n' % patch['note'])
        sys.stdout.write(separator)


# Parse command-line options.
disc = """
This utility applies patch files.
"""

parser = OptionParser(description=disc)
parser.add_option('--patch-config', dest='patchconfig', metavar='DIR',
                  help='patch configuration file')
parser.add_option('--patch-file', dest='patchfile', metavar='FILE',
                  help='patch source file')
parser.add_option('--patch-dir', dest='patchdir', metavar='DIR',
                  help='patch target directory')
(options, args) = parser.parse_args()

if not options.patchconfig is None:
  patch_config(options.patchconfig)
elif not options.patchfile is None and not options.patchdir is None:
  patch_file(options.patchfile, options.patchdir)
else:
  parser.print_help(sys.stdout)
  sys.exit()
