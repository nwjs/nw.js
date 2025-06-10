import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import shutil
import platform
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument('nwapp=' + testdir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    driver.implicitly_wait(5)
    result = driver.find_element(By.ID, 'result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
    result = driver.find_element(By.ID, 'result2')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    driver.quit()