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
nwdist = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwdist')

appdir =  os.path.join(testdir, 'app')
pkg1 = os.path.join(testdir, 'pkg1')
arch = os.path.join(pkg1, 'arch')

try:
    shutil.rmtree(pkg1)
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
    subprocess.check_output([os.path.join(pkg1, 'nw'), os.path.join(testdir, 'show-arch.js'), arch])
    with open(arch, 'r') as arch_file:
        return arch_file.read()

########### start test ############
# create test directory
#os.mkdir(pkg1)

# copy nw to test directory
print "copying %s to %s" % (nwdist, pkg1)
copytree(nwdist, pkg1)

# copy app & flash plugin to test directory
if platform.system() == 'Darwin':
    # check nwjs version
    versions = subprocess.check_output([os.path.join(pkg1, 'nwjs.app', 'Contents', 'MacOS', 'nwjs'), '--version'])
    pluginsrc = os.path.join(testdir, '../../data/PepperFlash/mac')
    appdest = os.path.join(pkg1, 'nwjs.app', 'Contents', 'Resources', 'app.nw')
    plugindest = os.path.join(pkg1, 'nwjs.app', 'Contents', 'Versions', versions.split()[1], 'nwjs Framework.framework', 'Internet Plug-Ins', 'PepperFlash')
else:
    if platform.system() == 'Linux':
        pluginsrc = os.path.join(testdir, "../../data/PepperFlash/linux_%s" % nw_arch())
    else:
        pluginsrc = os.path.join(testdir, "../../data/PepperFlash/win_%s" % nw_arch())
    appdest = pkg1
    plugindest = os.path.join(pkg1, 'PepperFlash')
print "copying %s to %s" % (appdir, appdest)
copytree(appdir, appdest)

print "copying %s to %s" % (pluginsrc, plugindest)
copytree(pluginsrc, plugindest)

# start testing server
os.chdir(appdest)
port = str(utils.free_port())
server = subprocess.Popen(['python', os.path.join(testdir, 'http-server.py'), port])

# create manifest for test
manifest = open(os.path.join(appdest, 'package.json'), 'w')
manifest.write('''
{
  "name": "flash-test",
  "main": "http://localhost:%s/index.html",
  "node-remote": ["<all_urls>"],
  "webview": {
    "partitions": [
        {
          "name": "trusted",
          "accessible_resources": [ "<all_urls>" ]
        }
     ]
  }
}
''' % (port))
manifest.close()

driver_path=os.path.join(pkg1, 'chromedriver')
driver = webdriver.Chrome(executable_path=driver_path)
driver.implicitly_wait(2)
try:
    print driver.current_url
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("success" in result.get_attribute('innerHTML'))
finally:
    server.terminate()
    driver.quit()
