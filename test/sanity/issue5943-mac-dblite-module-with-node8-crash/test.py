import time
import os
import shutil
import platform
import sys
import subprocess

if platform.system() != 'Darwin':
    print 'Skipped for non Mac platform'
    sys.exit(0)

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
node_module = os.path.join(testdir, "node_modules")
os.chdir(testdir)

install_native_modules()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url
    print "select data from table Australia"
    result = driver.find_element_by_id("shell").get_attribute("innerHTML")
    assert("Sausage" in result)
    print 'There is no crash'
finally:
    driver.quit()
