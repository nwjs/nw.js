import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
test_dir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument('nwapp=' + test_dir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element_by_tag_name('h1')
    ret = result.get_attribute('innerHTML')
    print(ret)
    assert 'NW.js' in result.get_attribute('innerHTML')
finally:
    driver.quit()