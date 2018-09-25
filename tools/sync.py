import subprocess as sp
import sys
import os
import re

revisions = {
    'nw_src_revision': ''
}

gclient_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
deps_file = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'DEPS'))

DEFAULT_CONFIG = """solutions = [{"managed": True, "name": "src", "url": "https://github.com/nwjs/chromium.src.git@%(nw_src_revision)s", "custom_deps": {}, "deps_file": "content/nw/DEPS", "safesync_url": "", "custom_vars": {}}]"""

f = open(deps_file)
for line in f:
    if re.match('.*nw_src_revision.*', line):
        revisions['nw_src_revision'] = line.split()[1].strip("',")

cmd = ['gclient', 'sync', '--with_branch_heads', '-f', '-v', '--spec', (DEFAULT_CONFIG % revisions)]

print cmd
print gclient_root

os.chdir(gclient_root)
sys.exit(sp.call(cmd))
