import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
capabilities = {"pageLoadStrategy": "none"}

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    print('waiting for crash')
    wait_switch_window_url(driver, 'index.html')
    result = wait_for_element_id(driver, 'versions')
    #print result
    assert('NWjs version' in result)
    print('There is no crash')
finally:
    driver.quit()
