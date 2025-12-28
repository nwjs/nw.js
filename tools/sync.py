from __future__ import print_function

import subprocess as sp
import sys
import os
import re
import argparse

revisions = {
    'nw_src_revision': ''
}

gclient_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
deps_file = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'DEPS'))
src_dir = os.path.join(gclient_root, 'src')

parser = argparse.ArgumentParser(description='Gclient sync script with optional shallow fetch.')
parser.add_argument('--shallow', action='store_true', help='Perform a shallow fetch of nw_src_revision.')
args = parser.parse_args()

if args.shallow:
    DEFAULT_CONFIG = """solutions = [{"managed": False, "name": "src", "url": "https://github.com/nwjs/chromium.src.git@%(nw_src_revision)s", "custom_deps": {}, "deps_file": "content/nw/DEPS", "safesync_url": "", "custom_vars": {"download_reclient": True, "non_git_source": True}}]"""
else:
    DEFAULT_CONFIG = """solutions = [{"managed": True, "name": "src", "url": "https://github.com/nwjs/chromium.src.git@%(nw_src_revision)s", "custom_deps": {}, "deps_file": "content/nw/DEPS", "safesync_url": "", "custom_vars": {"download_reclient": True, "non_git_source": True}}]"""

try:
    f = open(deps_file, 'r')
    try:
        for line in f:
            if 'nw_src_revision' in line:
                revisions['nw_src_revision'] = line.split()[1].strip("',")
                break
    finally:
        f.close()
except IOError as e:
    print("Error opening DEPS file: {}".format(e))
    sys.exit(1)

if not revisions['nw_src_revision']:
    print("Error: Could not find 'nw_src_revision' in DEPS file.")
    sys.exit(1)

if args.shallow:
    if not os.path.isdir(src_dir):
        print("Error: Directory {} does not exist. Run gclient sync without --shallow first to create it.".format(src_dir))
        sys.exit(1)

    print("--- Performing shallow fetch for revision: {} ---".format(revisions['nw_src_revision']))
    fetch_cmd = ['git', 'fetch', 'origin', revisions['nw_src_revision'], '--depth', '1']
    reset_cmd = ['git', 'reset', '--hard', 'FETCH_HEAD']

    print("Executing in {}: {}".format(src_dir, ' '.join(fetch_cmd)))
    fetch_ret = sp.call(fetch_cmd, cwd=src_dir, shell=(os.name == 'nt'))
    if fetch_ret != 0:
        print("Error: git fetch failed with exit code {}".format(fetch_ret))
        sys.exit(fetch_ret)

    print("Executing in {}: {}".format(src_dir, ' '.join(reset_cmd)))
    reset_ret = sp.call(reset_cmd, cwd=src_dir, shell=(os.name == 'nt'))
    if reset_ret != 0:
        print("Error: git reset failed with exit code {}".format(reset_ret))
        sys.exit(reset_ret)
    print("--- Shallow fetch completed ---")

cmd = ['gclient', 'sync', '--with_branch_heads', '-f', '--reset', '-v', '--spec', (DEFAULT_CONFIG % revisions)]

print("Original gclient command: {}".format(' '.join(cmd)))
print("Gclient root: {}".format(gclient_root))

current_dir = os.getcwd()
os.chdir(gclient_root)
print("Changed directory to: {}".format(gclient_root))

exit_code = sp.call(cmd, shell=(os.name == 'nt'))

os.chdir(current_dir)
print("Changed directory back to: {}".format(current_dir))

sys.exit(exit_code)
