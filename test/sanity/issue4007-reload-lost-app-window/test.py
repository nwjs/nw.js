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

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"], desired_capabilities = capabilities)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert("success" in result)
    old_handle = driver.current_window_handle
    driver.find_element_by_id('reloadapp').click()
    print('wait for app reload')
    wait_window_handles(driver, lambda handles: handles != None and len(handles) != 0 and handles[0] != old_handle)
    # devtools will be opened as the first window handle
    print(driver.window_handles)
    driver.switch_to.window(driver.window_handles[0])
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert("success" in result)
finally:
    driver.quit()
