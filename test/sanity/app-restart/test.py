import json
import os
import platform
import shutil
import subprocess
import time

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

appdir = testdir
datadir = os.path.join(testdir, 'userdata')
state = os.path.join(testdir, 'restart-state.json')
result = os.path.join(testdir, 'restart-result.json')

try:
    shutil.rmtree(datadir)
except OSError:
    pass

for path in [state, result]:
    try:
        os.remove(path)
    except OSError:
        pass

pkg = os.path.dirname(os.environ['CHROMEDRIVER'])

if platform.system() == 'Darwin':
    exe = os.path.join(pkg, 'nwjs.app', 'Contents', 'MacOS', 'nwjs')
elif platform.system() == 'Linux':
    exe = os.path.join(pkg, 'nw')
else:
    exe = os.path.join(pkg, 'nw.exe')

ret = subprocess.call([
    exe,
    '--nwjs-test-mode2',
    '--user-data-dir=' + datadir,
    '--restart-test-flag=present',
    appdir,
    'plain-restart-arg'
], env=dict(os.environ, APP_RESTART_TEST_DIR=testdir))
assert ret == 0

deadline = time.time() + 30
while not os.path.exists(result) and time.time() < deadline:
    time.sleep(1)

assert os.path.exists(result)

with open(result, 'r') as result_file:
    data = json.load(result_file)

print(data)
assert data['success']
