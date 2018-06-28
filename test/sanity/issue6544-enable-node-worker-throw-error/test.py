import os
import sys
import time

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url
    wait_window_handles(driver, 2)
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "wait 2 minutes to throw new error"
    time.sleep(2)
    print "check if there is an error whether throwed"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    if len(elems) > 1:
        for i in range(len(elems)):
            if "ErrorEvent" in elems[i].get_attribute("innerHTML"):
                output = elems[i].get_attribute("innerHTML")
                break
    else:
        output = elems[0].get_attribute("innerHTML")
    print output
    assert("ErrorEvent" in output)
finally:
    driver.quit()
