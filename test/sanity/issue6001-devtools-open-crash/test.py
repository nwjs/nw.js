import time
import os
import shutil
import subprocess
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
binfile = os.path.join(testdir, "index.bin")
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']),
       "nwjc.exe" if os.name == "nt" else "nwjc")
os.chdir(testdir)

try:
  os.remove(binfile)
except:
  pass

assert(False == os.path.isfile(binfile))
subprocess.call([nwjc, "--future", "index.js", "index.bin"])
assert(os.path.isfile(binfile))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print driver.current_url
    print 'waiting for devtools open'
    wait_window_handles(driver, 2)
    print 'switch to devtools'
    switch_to_devtools(driver, None, True)
    print 'waiting for crash'
    time.sleep(5)
    handles = driver.window_handles
    assert(len(handles) is 2)
    print 'There is no crash'
finally:
    driver.quit()
