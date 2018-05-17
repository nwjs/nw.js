import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
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
    print "check if 'this' whether point to the nw.Window object."
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    for i in range(len(elems)):
        if "win === this ? true" in elems[i].get_attribute("innerHTML"):
            output = elems[i].get_attribute("innerHTML")
            break
    print output
    assert("win === this ? true" in output)
finally:
    driver.quit()
