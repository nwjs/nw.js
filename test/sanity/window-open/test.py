import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
capabilities = {'pageLoadStrategy': 'none'}
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log', additional_capabilities=capabilities)
try:
    wait_switch_window_name(driver, 'local')
    print(driver.current_url)
    driver.find_element(By.ID, 'open').click()
    result = wait_for_element_id(driver, 'res')
    print('result=' + result)
    assert 'object' in result
    wait_switch_window_name(driver, 'remote')
    result = wait_for_element_id(driver, 'res')
    print(result)
    assert 'ENABLED' in result
finally:
    driver.quit()