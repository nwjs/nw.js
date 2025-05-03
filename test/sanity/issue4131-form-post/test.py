import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    driver.implicitly_wait(10)
    print(driver.current_url)
    driver.execute_script('startServer()')
    result = wait_for_element_id(driver, 'server-started')
    print('server started')
    driver.find_element_by_id('submit').click()
    result = wait_for_element_id(driver, 'reqno')
    print(result)
    result = driver.find_element_by_id('method').get_attribute('innerHTML')
    print(result)
    assert 'POST' in result
    driver.find_element_by_id('submit').click()
    result = wait_for_element_id_content(driver, 'reqno', 'reqno: 1')
    print(result)
    result = driver.find_element_by_id('method').get_attribute('innerHTML')
    print(result)
    assert 'POST' in result
    result = driver.find_element_by_id('data').get_attribute('innerHTML')
    print(result)
    assert 'myvalue2' in result
finally:
    driver.quit()
