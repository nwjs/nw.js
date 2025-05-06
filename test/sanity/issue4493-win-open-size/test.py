import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import platform
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
print('Open first time')
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(10)
try:
    print(driver.current_url)
    size = driver.find_element_by_id('size').get_attribute('innerHTML')
    print('open size %s' % size)
    driver.find_element_by_id('resize-window').click()
    size = driver.find_element_by_id('resize').get_attribute('innerHTML')
    print('resize to %s' % size)
finally:
    driver.quit()
print('Open second time')
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(10)
try:
    print(driver.current_url)
    size = driver.find_element_by_id('size').get_attribute('innerHTML')
    print('open size %s' % size)
    assert size == '666x333' or size == '667x334'
finally:
    driver.quit()