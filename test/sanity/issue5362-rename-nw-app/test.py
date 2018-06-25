import time
import os
import shutil
import subprocess
import platform
import sys

if platform.system() != "Windows":
    print 'Skipped for non Windows platform'
    sys.exit(0)

test_dir = os.path.dirname(os.path.abspath(__file__))

driver_dir = os.path.dirname(os.environ['CHROMEDRIVER'])
new_sdk_dir = os.path.join(os.path.dirname(driver_dir), "sdk-issue5362")
txt_p = os.path.join(test_dir, "issue5362.txt")

try:
    shutil.rmtree(new_sdk_dir)
    shutil.rmfile(txt_p)
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

print "copying %s to %s" % (driver_dir, new_sdk_dir)
copytree(driver_dir, new_sdk_dir)

def readfile():
    fp = txt_p
    f = open(fp, "r")
    return f.read()

print "#### test rename nw.exe to newNw.exe ####"
os.chdir(new_sdk_dir)
os.rename("nw.exe", "newNw.exe")
os.chdir(test_dir)

new_nw_path = os.path.join(new_sdk_dir, "newNw.exe")

server = subprocess.Popen([new_nw_path, test_dir], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
try:
    print "wait one minute to launch app"
    time.sleep(1)
    res = readfile()
    print res
    assert("Test page" in res)
finally:
    server.terminate()