import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
chrome_options.add_argument('nwapp=' + testdir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    driver.quit()