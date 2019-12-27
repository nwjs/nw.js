import time
import os
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
os.chdir(testdir)

install_native_modules()

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
    devtools_click_tab(driver, 'sources')
    print 'start to debug line 16 via breakpoint'
    for i in range(10):
        print "click Resume script execution button: %s" % i 
        j = 0
        while j < 10:
            try:
                clickResume()
                break
            except selenium.common.exceptions.WebDriverException:
                pass
            time.sleep(1)
            j = j + 1
            if j >= 10:
                raise Exception('Timeout when waiting for clicking resume button')
    assert(len(driver.window_handles) is 2)
    print 'There is no crash'
finally:
    driver.quit()
