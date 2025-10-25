# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file

from exec_util import exec_cmd
import os
import sys

if sys.platform == 'win32':
  # Force use of the git version bundled with depot_tools.
  git_exe = 'git.bat'
else:
  git_exe = 'git'


def is_checkout(path):
  """ Returns true if the path represents a git checkout. """
  return os.path.isdir(os.path.join(path, '.git'))


def get_hash(path='.', branch='HEAD'):
  """ Returns the git hash for the specified branch/tag/hash. """
  cmd = "%s rev-parse %s" % (git_exe, branch)
  result = exec_cmd(cmd, path)
  if result['out'] != '':
    return result['out'].strip()
  return 'Unknown'


def get_url(path='.'):
  """ Returns the origin url for the specified path. """
  cmd = "%s config --get remote.origin.url" % git_exe
  result = exec_cmd(cmd, path)
  if result['out'] != '':
    return result['out'].strip()
  return 'Unknown'


def get_commit_number(path='.', branch='HEAD'):
  """ Returns the number of commits in the specified branch/tag/hash. """
  cmd = "%s rev-list --count %s" % (git_exe, branch)
  result = exec_cmd(cmd, path)
  if result['out'] != '':
    return result['out'].strip()
  return '0'


def get_changed_files(path, hash):
  """ Retrieves the list of changed files. """
  if hash == 'unstaged':
    cmd = "%s diff --name-only" % git_exe
  elif hash == 'staged':
    cmd = "%s diff --name-only --cached" % git_exe
  else:
    cmd = "%s diff-tree --no-commit-id --name-only -r %s" % (git_exe, hash)
  result = exec_cmd(cmd, path)
  if result['out'] != '':
    files = result['out']
    if sys.platform == 'win32':
      # Convert to Unix line endings.
      files = files.replace('\r\n', '\n')
    return files.strip().split("\n")
  return []


def write_indented_output(output):
  """ Apply a fixed amount of intent to lines before printing. """
  if output == '':
    return
  for line in output.split('\n'):
    line = line.strip()
    if len(line) == 0:
      continue
    sys.stdout.write('\t%s\n' % line)


def git_apply_patch_file(patch_path, patch_dir):
  """ Apply |patch_path| to files in |patch_dir|. """
  patch_name = os.path.basename(patch_path)
  sys.stdout.write('\nApply %s in %s\n' % (patch_name, patch_dir))

  if not os.path.isfile(patch_path):
    sys.stdout.write('... patch file does not exist.\n')
    return 'fail'

  patch_string = open(patch_path, 'rb').read().decode('utf-8')
  if sys.platform == 'win32':
    # Convert the patch to Unix line endings. This is necessary to avoid
    # whitespace errors with git apply.
    patch_string = patch_string.replace('\r\n', '\n')

  # Git apply fails silently if not run relative to a respository root.
  if not is_checkout(patch_dir):
    sys.stdout.write('... patch directory is not a repository root.\n')
    return 'fail'

  # Output patch contents.
  cmd = '%s apply -p0 --numstat' % git_exe
  result = exec_cmd(cmd, patch_dir, patch_string)
  write_indented_output(result['out'].decode('utf-8').replace('<stdin>', patch_name))

  # Reverse check to see if the patch has already been applied.
  cmd = '%s apply -p0 --reverse --check' % git_exe
  result = exec_cmd(cmd, patch_dir, patch_string)
  if result['err'].decode('utf-8').find('error:') < 0:
    sys.stdout.write('... already applied (skipping).\n')
    return 'skip'

  # Normal check to see if the patch can be applied cleanly.
  cmd = '%s apply -p0 --check' % git_exe
  result = exec_cmd(cmd, patch_dir, patch_string)
  if result['err'].decode('utf-8').find('error:') >= 0:
    sys.stdout.write('... failed to apply:\n')
    write_indented_output(result['err'].replace('<stdin>', patch_name))
    return 'fail'

  # Apply the patch file. This should always succeed because the previous
  # command succeeded.
  cmd = '%s apply -p0' % git_exe
  result = exec_cmd(cmd, patch_dir, patch_string)
  if result['err'].decode('utf-8') == '':
    sys.stdout.write('... successfully applied.\n')
  else:
    sys.stdout.write('... successfully applied (with warnings):\n')
    write_indented_output(result['err'].replace('<stdin>', patch_name))
  return 'apply'
