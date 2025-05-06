import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    driver.quit()