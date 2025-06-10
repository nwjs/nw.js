import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    result = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
    print(result)
    assert 'object' == result
    result2 = driver.find_element(By.ID, 'result2').get_attribute('innerHTML')
    print(result2)
    assert 'undefined' == result2
finally:
    driver.quit()