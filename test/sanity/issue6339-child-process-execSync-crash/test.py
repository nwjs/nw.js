import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    res = wait_for_element_id(driver, 'result')
    print(res)
    assert 'v' in res
    driver.find_element(By.ID, 'btn').click()
    print('waiting for crash')
    time.sleep(3)
    result = wait_for_element_id(driver, 'result')
    assert 'v' in result
    print('There is no crash')
finally:
    driver.quit()