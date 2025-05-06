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
testdir = os.path.dirname(os.path.abspath(__file__))
nwdist = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwdist')
appdir = os.path.join(testdir, 'app')
pkg1 = os.path.join(testdir, 'pkg1')
pkg2 = os.path.join(testdir, 'pkg2')
pkg3 = os.path.join(testdir, 'pkg3')
try:
    shutil.rmtree(pkg1)
    shutil.rmtree(pkg2)
    shutil.rmtree(pkg3)
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
    appnw = os.path.join(pkg1, 'nwjs.app', 'Contents', 'Resources', 'app.nw')
    print('copying %s to %s' % (appdir, appnw))
    copytree(appdir, appnw)
else:
    print('copying %s to %s' % (appdir, pkg1))
    copytree(appdir, pkg1)
driver_path = os.path.join(pkg1, 'chromedriver')
driver = get_configured_webdriver(driver_path=driver_path)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    driver.quit()
os.mkdir(pkg2)
print('copying %s to %s' % (nwdist, pkg2))
copytree(nwdist, pkg2)
if platform.system() == 'Darwin':
    appnw = os.path.join(pkg2, 'nwjs.app', 'Contents', 'Resources', 'app.nw')
    print('compressing %s to %s' % (appdir, appnw))
    compress(appdir, appnw)
else:
    package_nw = os.path.join(pkg2, 'package.nw')
    print('compressing %s to %s' % (appdir, package_nw))
    compress(appdir, package_nw)
driver_path = os.path.join(pkg2, 'chromedriver')
driver2 = get_configured_webdriver(driver_path=driver_path)
driver2.implicitly_wait(5)
try:
    print(driver2.current_url)
    result = driver2.find_element_by_id('result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    driver2.quit()
if platform.system() != 'Darwin':
    os.mkdir(pkg3)
    print('copying %s to %s' % (nwdist, pkg3))
    copytree(nwdist, pkg3)
    package_nw = os.path.join(pkg3, 'package.nw')
    print('compressing %s to %s' % (appdir, package_nw))
    compress(appdir, package_nw)
    if platform.system() == 'Linux':
        nwbin = os.path.join(pkg3, 'nw')
    else:
        nwbin = os.path.join(pkg3, 'nw.exe')
    with open(nwbin, 'ab') as myfile, open(package_nw, 'rb') as file2:
        myfile.write(file2.read())
    os.remove(package_nw)
    driver_path = os.path.join(pkg3, 'chromedriver')
    driver3 = get_configured_webdriver(driver_path=driver_path)
    time.sleep(1)
    try:
        print(driver3.current_url)
        result = driver3.find_element_by_id('result')
        print(result.get_attribute('innerHTML'))
        assert 'success' in result.get_attribute('innerHTML')
    finally:
        driver3.quit()
