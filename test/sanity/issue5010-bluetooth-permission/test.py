import time
import os
import sys
import platform
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
if platform.system() != 'Linux':
    sys.exit(0)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    result = wait_for_element_id(driver, 'result')
    print(result)
    assert 'Permission denied' not in result
finally:
    driver.quit()