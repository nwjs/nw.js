#!/usr/bin/env python
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Generate a CL to roll a DEPS entry to the specified revision number and post
it for review so that the CL will land automatically if it passes the
commit-queue's checks.
"""

import logging
import optparse
import os
import re
import sys

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
SRC_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, os.pardir, os.pardir, os.pardir))
sys.path.insert(0, os.path.join(SRC_DIR, 'build'))
import find_depot_tools
import scm
import subprocess

IS_WINDOWS = sys.platform.startswith('win')

def die_with_error(msg):
  print >> sys.stderr, msg
  sys.exit(1)


def process_deps(path, project, new_rev, is_dry_run):
  """Update project_revision to |new_issue|.

  A bit hacky, could it be made better?
  """
  content = open(path).read()
  # Hack for Blink to get the AutoRollBot running again.
  if project == "blink":
    project = "webkit"
  old_line = r"(\s+)'%s_revision': '([0-9a-f]{2,40})'," % project
  new_line = r"\1'%s_revision': '%s'," % (project, new_rev)
  new_content = re.sub(old_line, new_line, content, 1)
  old_rev = re.search(old_line, content).group(2)
  if not old_rev:
    die_with_error('Failed to update the DEPS file')

  if not is_dry_run and new_content != content:
    open(path, 'w').write(new_content)
  return old_rev


class PrintSubprocess(object):
  """Wrapper for subprocess2 which prints out every command."""
  def __getattr__(self, attr):
    def _run_subprocess2(cmd, *args, **kwargs):
      print cmd
      sys.stdout.flush()
      return getattr(subprocess2, attr)(cmd, *args, **kwargs)
    return _run_subprocess2

prnt_subprocess = PrintSubprocess()


def main():
  tool_dir = os.path.dirname(os.path.abspath(__file__))
  parser = optparse.OptionParser(usage='%prog [options] <project> <new rev>',
                                 description=sys.modules[__name__].__doc__)
  parser.add_option('-v', '--verbose', action='count', default=0)
  parser.add_option('--dry-run', action='store_true')
  parser.add_option('-f', '--force', action='store_true',
                    help='Make destructive changes to the local checkout if '
                         'necessary.')
  parser.add_option('--commit', action='store_true', default=True,
                    help='(default) Put change in commit queue on upload.')
  parser.add_option('--no-commit', action='store_false', dest='commit',
                    help='Don\'t put change in commit queue on upload.')
  parser.add_option('-r', '--reviewers', default='',
                    help='Add given users as either reviewers or TBR as'
                    ' appropriate.')
  parser.add_option('--upstream', default='origin/master',
                    help='(default "%default") Use given start point for change'
                    ' to upload. For instance, if you use the old git workflow,'
                    ' you might set it to "origin/trunk".')
  parser.add_option('--cc', help='CC email addresses for issue.')
  parser.add_option('-m', '--message', help='Custom commit message.')

  options, args = parser.parse_args()
  logging.basicConfig(
      level=
          [logging.WARNING, logging.INFO, logging.DEBUG][
            min(2, options.verbose)])
  #if len(args) != 2:
  #  parser.print_help()
  #  exit(0)

  root_dir = os.path.dirname(tool_dir)
  os.chdir(root_dir)

  projects = [('src', '.'), ('v8', 'v8'), ('node', 'third_party/node-nw')]
  for project, path in projects:
    os.chdir(os.path.join(SRC_DIR, path))
    new_rev = subprocess.check_output(['git', 'rev-parse', 'HEAD'], shell=IS_WINDOWS).rstrip()

    # Silence the editor.
    os.environ['EDITOR'] = 'true'

    old_rev = process_deps(os.path.join(root_dir, 'DEPS'), 'nw_' + project, new_rev,
                           options.dry_run)
    if old_rev == new_rev:
      continue
    print '%s roll %s:%s' % (project, old_rev, new_rev)
    commit_msg = subprocess.check_output(['git', 'log', '-1', '--format=%s'], shell=IS_WINDOWS).rstrip()
    commit_msg = project + ": " + commit_msg
    os.chdir(root_dir)
    subprocess.check_output(['git', 'add', 'DEPS'], shell=IS_WINDOWS)
    subprocess.check_output(['git', 'commit', '--quiet', '-m', commit_msg], shell=IS_WINDOWS)
    break
  return 0


if __name__ == '__main__':
  sys.exit(main())
