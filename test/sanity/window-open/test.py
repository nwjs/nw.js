import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

capabilities = {"pageLoadStrategy": "none"}

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
try:
    wait_switch_window_name(driver, 'local')
    print(driver.current_url)
    driver.find_element_by_id('open').click()
    result = wait_for_element_id(driver, 'res')
    print('result=' + result)
    assert("object" in result)
    wait_switch_window_name(driver, 'remote')
    result = wait_for_element_id(driver, 'res')
    print(result)
    assert("ENABLED" in result)
finally:
    driver.quit()

