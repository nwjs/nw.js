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
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element(By.ID, 'inject_start').get_attribute('innerHTML')
    print('inject_js_start: %s' % result)
    assert 'success' in result
    elems = driver.find_elements(By.TAG_NAME, 'h1')
    assert len(elems) == 1
    driver.switch_to.frame(driver.find_element(By.TAG_NAME, 'iframe'))
    result = driver.find_element(By.ID, 'inject_start').get_attribute('innerHTML')
    print('inject_js_start in iframe: %s' % result)
    assert 'success' in result
    elems = driver.find_elements(By.TAG_NAME, 'h1')
    assert len(elems) == 1
finally:
    driver.quit()