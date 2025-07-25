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
try:
    wait_switch_window_name(driver, 'index')
    print(driver.current_url)
    result = wait_for_element_id(driver, 'result')
    result2 = wait_for_element_id(driver, 'result2')
    print(result)
    print(result2)
    assert result == 'success from popup' and result2 == 'startiframe'
finally:
    driver.quit()