import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    inner_size = wait_for_element_id(driver, 'yellow')
    print('inner size: %s' % inner_size)
    outer_size = wait_for_element_id(driver, 'ret')
    print('outer size: %s' % outer_size)
    assert inner_size == outer_size
finally:
    driver.quit()