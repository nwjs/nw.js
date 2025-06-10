import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
chrome_options.add_argument('nwapp=' + testdir)
chrome_options.add_argument('user-data-dir=' + os.path.join(testdir, 'userdata'))
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    second_instance = subprocess.Popen(['python3', 'second_instance.py'])
    result = wait_for_element_id_content(driver, 'result', 'success')
    print(result)
finally:
    driver.quit()