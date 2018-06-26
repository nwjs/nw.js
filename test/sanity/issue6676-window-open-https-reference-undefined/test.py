import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    print driver.current_url
    print "wait for devtools open"
    wait_window_handles(driver, 3)
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check output message in console panel"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    if len(elems) > 2:
        for i in range(len(elems)):
            output.append(elems[i].get_attribute("innerHTML"))
    else:
        output = elems[0].get_attribute("innerHTML")
    print output
    assert("NWWindow" in output)

finally:
    driver.quit()