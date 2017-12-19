# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os
import subprocess
import sys

# This script creates a TAR file for archiving NW symbols.

def Main(args):
  if len(args) < 2:
    print >> sys.stderr, "Usage: python archive_symbols.py file.tar file..."
    return 1

  _RemoveIfExists(args[0])

  try:
    return subprocess.check_call(['tar', '-cf'] + args)
  except:
    _RemoveIfExists(args[0])
    raise


def _RemoveIfExists(path):
  if os.path.exists(path):
    os.unlink(path)


if __name__ == '__main__':
  sys.exit(Main(sys.argv[1:]))
