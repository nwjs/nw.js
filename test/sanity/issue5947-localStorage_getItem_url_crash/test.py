import time
import os
import platform
import sys
if platform.system() == 'Darwin':
    print('Skipped for Mac platform')
    sys.exit(0)
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    res = wait_for_element_id_content(driver, 'result', 'URL is: null')
    print(res)
    assert 'URL is: null' in res
    print('There is no crash')
finally:
    driver.quit()