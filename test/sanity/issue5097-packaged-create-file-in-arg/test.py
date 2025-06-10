import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import shutil
import zipfile
import platform
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
testdir = os.path.dirname(os.path.abspath(__file__))
nwdist = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwdist')
appdir = os.path.join(testdir, 'app')
pkg1 = os.path.join(testdir, 'pkg1')
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
            z.write(_path, _path.replace(from_dir + os.sep, ''))
    z.close()

def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        elif not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
            shutil.copy2(s, d)
os.mkdir(pkg1)
print('copying %s to %s' % (nwdist, pkg1))
copytree(nwdist, pkg1)
if platform.system() == 'Darwin':
    appdest = os.path.join(pkg1, 'nwjs.app', 'Contents', 'Resources', 'app.nw')
else:
    appdest = pkg1
print('copying %s to %s' % (appdir, appdest))
copytree(appdir, appdest)
os.chdir(pkg1)
unexpected_file = 'unexpected_file'
assert not os.path.exists(unexpected_file), "'%s' should not be existed before testing" % unexpected_file
chrome_options = Options()
chrome_options.add_nw_argument(unexpected_file)
driver_path = os.path.join(pkg1, 'chromedriver.exe' if sys.platform == 'win32' else 'chromedriver')
driver = get_configured_webdriver(chrome_options_instance=chrome_options, driver_path=driver_path)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element(By.ID, 'result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    driver.quit()
assert not os.path.exists(unexpected_file), "'%s' should not be existed after testing" % unexpected_file
