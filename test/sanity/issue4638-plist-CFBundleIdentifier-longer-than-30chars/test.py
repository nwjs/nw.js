import os
import sys
import time
import shutil
import platform

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

if platform.system() != "Darwin":
    print "Skipped for not Mac platform"
    sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
test_dir = os.path.dirname(os.path.abspath(__file__))

driver_dir = os.path.dirname(os.environ["CHROMEDRIVER"])
target_dir = os.path.join(driver_dir, "nwjs.app", "Contents", "Resources", "4638")
move_dir = os.path.join(test_dir, "app")
cache_dir = os.path.join(os.getenv("HOME"), "Library", "Application Support", "4638")
plist_f = os.path.join(driver_dir, "nwjs.app", "Contents", "Info.plist")
plist_f_new = os.path.join(driver_dir, "nwjs.app", "Contents", "Info.plist.new")
new_sdk_dir = os.path.join(os.path.dirname(driver_dir), "issue4368")
new_driver_dir = os.path.join(new_sdk_dir, "chromedriver")

try:
    shutil.rmtree(target_dir)
    shutil.rmtree(cache_dir)
    shutil.rmtree(new_sdk_dir)
except:
    pass

def update_plist():
    flag = 0

    plist = open(plist_f, "r+")
    new_plist = open(plist_f_new, "w+")
    for i, line in enumerate(plist):
        if flag == 0:
            new_plist.writelines(line)
        elif flag == 1:
            flag =0
            old_line = line
            new_line = "\t<string>uk.org.wanderinghippo.1234567890</string>\n"
            content = old_line.replace(old_line, new_line)
            print content
            new_plist.writelines(content)

        if "<key>CFBundleIdentifier</key>" in line:
            flag = 1
 
    plist.close()
    new_plist.close()
    shutil.move(plist_f_new, plist_f)

def clear_sdk():
    print "move new sdk folder issue4638"
    shutil.rmtree(new_sdk_dir)

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

print "copying sdk %s to %s" % (driver_dir, new_sdk_dir) 
copytree(driver_dir, new_sdk_dir)
print "copying test app %s to %s" % (move_dir, target_dir)
copytree(move_dir, target_dir)

print "update Info.plist file"
update_plist()

chrome_options.add_argument("nwapp=" + target_dir)

driver = webdriver.Chrome(executable_path=new_driver_dir, chrome_options=chrome_options)
driver.implicitly_wait(2)

try:
    print driver.current_url
    print "wait for crash"
    time.sleep(5)
    res = wait_for_element_id_content(driver, "versions", "NWjs version")
    assert("NWjs version" in res)
    print "There is no crash"
    clear_sdk()
finally:
    driver.quit()
