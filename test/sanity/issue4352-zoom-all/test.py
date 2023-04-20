import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

capabilities = {"pageLoadStrategy": "none"}

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
try:
    wait_switch_window_url(driver, 'index.html')
    driver.find_element_by_id('create-popup').click()
    wait_for_element_id_content(driver, 'result', 'created')
    wait_window_handles(driver, 2)
    wait_switch_window_url(driver, 'popup.html')
    #print(driver.current_url)
    zoom2 = 'zoom level'
    while zoom2 == 'zoom level':
        zoom2 = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
        time.sleep(1)
    print("zoom2: ", zoom2)
    wait_switch_window_name(driver, 'index')
    driver.find_element_by_id('get-zoom').click()
    zoom1 = driver.find_element_by_id('result').get_attribute('innerHTML')
    print("zoom1: ", zoom1)
    assert(zoom1 == '1' or zoom1 == '1.5')
    assert(zoom2 != '1')
finally:
    driver.quit()
