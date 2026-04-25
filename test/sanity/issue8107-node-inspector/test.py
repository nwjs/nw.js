import time
import os
import sys
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    wait_window_handles(driver, 1)
    driver.switch_to.window(driver.window_handles[0])
    result = wait_for_element_id_content(driver, 'result', 'PASS', timeout=10)
    assert 'PASS' in result, 'Expected PASS but got: %s' % result
    print('PASS: node:inspector module is available')
finally:
    driver.quit()
