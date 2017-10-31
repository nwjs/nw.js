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
driver.implicitly_wait(5)
time.sleep(1)
try:
    wait_window_handles(driver, 2)
    wait_switch_window_name(driver, 'index')
    print driver.current_url
    result = wait_for_element_id(driver, 'inject_start')
    print 'inject_js_start: %s' % result
    assert('success' in result)
    result = wait_for_element_id(driver, 'inject_end')
    print 'inject_js_end: %s' % result
    assert('success' in result)
finally:
    driver.quit()
