import time
import os
import shutil
import sys
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'result', 'success')
    print(result)
    assert 'success' in result
finally:
    driver.quit()