import os
import sys
import platform
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.by import By
if platform.system() == 'Windows':
    print('Skipped for Windows platform')
    sys.exit(0)
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'result', 'CAUGHT')
    assert 'CAUGHT' in result
    print('Got output from index page')
finally:
    driver.quit()