import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
from selenium import webdriver
driver = get_configured_webdriver()
try:
    print(driver.current_url)
    id = driver.find_element_by_id('nwjsver')
    print(id.text)
finally:
    driver.quit()
