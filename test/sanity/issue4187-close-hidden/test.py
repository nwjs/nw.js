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
try:
    print driver.current_url
    print 'wait for window open'
    wait_window_handles(driver, 2)
    print 'Wait for splash window close'
    wait_window_handles(driver, 1)
    print driver.window_handles
    driver.switch_to_window(driver.window_handles[0])
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
finally:
    driver.quit()
