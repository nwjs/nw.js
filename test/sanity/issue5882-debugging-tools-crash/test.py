import time
import os
import shutil
import subprocess
import platform
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys 
from selenium.webdriver.support.select import Select

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
node_module = os.path.join(testdir, "node_modules")
os.chdir(testdir)

try:
    shutil.rmtree(node_module)
except:
    pass

assert(False == os.path.isdir(node_module))

if platform.system() == "Windows":
    subprocess.call(["npm", "install", "request"], shell=True)
else:
    subprocess.call(["npm", "install", "request"])

assert(os.path.isdir(node_module))

def clickResume():
    if platform.system() != "Darwin":
        ActionChains(driver).key_down(Keys.CONTROL).send_keys("\\").key_up(Keys.CONTROL).perform()
    else:
        ActionChains(driver).key_down(Keys.COMMAND).send_keys("\\").key_up(Keys.COMMAND).perform()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print driver.current_url
    print 'wait for devtools open'
    wait_window_handles(driver, 2)
    print 'switch to devtools'
    switch_to_devtools(driver)
    print "click Sources panel"
    time.sleep(1)
    devtools_click_tab(driver, 'sources')
    print 'start to debug line 16 via breakpoint'
    for i in range(10):
        print "click Resume script execution button: %s" % i 
        while i < 10:
            try:
                clickResume()
                break
            except selenium.common.exceptions.WebDriverException:
                pass
            time.sleep(1)
            i = i + 1
            if i >= 10:
                raise Exception('Timeout when waiting for clicking resume button')
    assert(len(driver.window_handles) is 2)
    print 'There is no crash'
finally:
    driver.quit()
