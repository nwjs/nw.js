import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
capabilities = {'pageLoadStrategy': 'none'}
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log', additional_capabilities=capabilities)
driver.implicitly_wait(2)
try:
    print('waiting for crash')
    wait_switch_window_url(driver, 'index.html')
    result = wait_for_element_id(driver, 'versions')
    assert 'NWjs version' in result
    print('There is no crash')
finally:
    driver.quit()