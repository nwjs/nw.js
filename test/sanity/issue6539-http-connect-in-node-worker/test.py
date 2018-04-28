import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print driver.current_url
    print "wait for devtools open"
    wait_window_handles(driver, 2)
    print driver.window_handles
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check if event 'connect' whether fired"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    for i in range(len(elems)):
        if "connected" in elems[i].get_attribute("innerHTML"):
            output = elems[i].get_attribute("innerHTML")
            break
    print output
    assert("connected" in output)
finally:
    driver.quit()
