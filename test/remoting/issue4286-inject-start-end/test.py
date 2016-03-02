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
    print driver.current_url
    wait_window_handles(driver, 2)
    result = driver.find_element_by_id('inject_start').get_attribute('innerHTML')
    print 'inject_js_start: %s' % result
    assert('success' in result)
    result = driver.find_element_by_id('inject_end').get_attribute('innerHTML')
    print 'inject_js_end: %s' % result
    assert('success' in result)
finally:
    driver.quit()
