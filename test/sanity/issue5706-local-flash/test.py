import time
import os
import shutil
import zipfile
import platform
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

testdir = os.path.dirname(os.path.abspath(__file__))
builddir = os.path.dirname(os.environ['CHROMEDRIVER'])
nwdist = os.path.join(builddir, 'nwdist')
plugindir = os.path.join(builddir, 'PepperFlash')

arch = os.path.join(testdir, 'arch')

if platform.system() != 'Linux':
    print 'Skipped for non Linux platform'
    sys.exit(0)

try:
    shutil.rmtree(plugindir)
except:
    pass

def compress(from_dir, to_file):
    from_dir = os.path.normpath(from_dir)
    to_file = os.path.normpath(to_file)
    
    z = zipfile.ZipFile(to_file, 'w', compression=zipfile.ZIP_DEFLATED)
    for root, dirs, files in os.walk(from_dir):
        for f in files:
            _path = os.path.join(root, f)
            z.write(_path, _path.replace(from_dir+os.sep, ''))
    z.close()

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

def nw_arch():
    subprocess.check_output([os.path.join(builddir, 'nw'), os.path.join(testdir, 'show-arch.js'), arch])
    with open(arch, 'r') as arch_file:
        return arch_file.read()

# copy app & flash plugin to test directory
pluginsrc = os.path.join(testdir, "../../data/PepperFlash/linux_%s" % nw_arch())

print "copying %s to %s" % (pluginsrc, plugindir)
copytree(pluginsrc, plugindir)

subprocess.check_call([os.path.join(builddir, 'browser_tests'), '--gtest_filter=*LocalFlash*', '--disable-gpu'])
