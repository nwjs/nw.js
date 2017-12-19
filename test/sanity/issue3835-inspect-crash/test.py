import time
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
time.sleep(1)
try:
    switch_to_app(driver)
    print driver.current_url
    print 'wait for devtools open'
    wait_window_handles(driver, 2)
    print driver.window_handles
    print 'switch to devtools'
    switch_to_devtools(driver)
    print 'click Console panel'
    devtools_click_tab(driver, 'console')
    print 'send_keys "chrome<enter>"'
    devtools_type_in_console(driver, 'chrome\n')
    driver.find_element_by_class_name('console-object-preview').click()
    time.sleep(1) # wait for crash!
    expanded = driver.find_element_by_css_selector('.console-view-object-properties-section.expanded')
    assert(expanded is not None)
finally:
    driver.quit()
