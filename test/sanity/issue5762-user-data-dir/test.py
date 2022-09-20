import time
import os
import shutil
import zipfile
import platform
import subprocess

testdir = os.path.dirname(os.path.abspath(__file__))
nwdist = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwdist')

appdir =  os.path.join(testdir, 'app')
pkg1 = os.path.join(testdir, 'pkg1')

try:
    shutil.rmtree(pkg1)
except:
    pass

def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            if not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
                shutil.copy2(s, d)

# create test directory
os.mkdir(pkg1)

# copy nw to test directory
print("copying %s to %s" % (nwdist, pkg1))
copytree(nwdist, pkg1)
pkgname = 'issue5762'

# copy app to test directory
if platform.system() == 'Darwin':
    appdest = os.path.join(pkg1, 'nwjs.app', 'Contents', 'Resources', 'app.nw')
    user_data_dir = os.path.join(os.getenv('HOME'), 'Library', 'Application Support', pkgname)
    exe = os.path.join(pkg1, 'nwjs.app', 'Contents', 'MacOS', 'nwjs')
    check_file = os.path.join(user_data_dir, 'Default', 'Web Data')
elif platform.system() == 'Linux':
    appdest = pkg1
    user_data_dir = os.path.join(os.getenv('HOME'), '.config', pkgname)
    exe = os.path.join(pkg1, 'nw')
    check_file = os.path.join(user_data_dir, 'Default', 'Web Data')
else:
    appdest = pkg1
    user_data_dir = os.path.join(os.getenv('LOCALAPPDATA'), pkgname)
    check_file = os.path.join(user_data_dir, 'User Data', 'Default', 'Web Data')
    exe = os.path.join(pkg1, 'nw.exe')

print("copying %s to %s" % (appdir, appdest))
copytree(appdir, appdest)

print("user data dir: %s" % (user_data_dir))
try:
    shutil.rmtree(user_data_dir)
except:
    pass

os.chdir(pkg1)

assert not os.path.exists(user_data_dir), "'%s' should not be existed before testing" % user_data_dir

p = subprocess.Popen([exe])
time.sleep(10)

user_data_dir_exists = False
check_file_exists = False

timeout = 30
while timeout > 0:
    if os.path.exists(user_data_dir):
        user_data_dir_exists = True
        break
    time.sleep(1)
    timeout = timeout - 1

if user_data_dir_exists:
    timeout = 30
    while timeout > 0:
        if os.path.exists(check_file):
            check_file_exists = True
            break
        time.sleep(1)
        timeout = timeout - 1
try:
    assert user_data_dir_exists
    assert check_file_exists
finally:
    #workaround stale renderer process if nw is killed early in startup
    if platform.system() == 'Windows':
        time.sleep(10)
    p.terminate()
