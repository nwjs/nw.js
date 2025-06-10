import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_experimental_option('windowTypes', ['webview'])
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    elems = driver.find_element(By.TAG_NAME, 'a')
    elems.click()
    result = wait_for_element_id_content(driver, 'result', 'success')
    print('result: %s' % result)
    assert 'nwjs.io' in result
finally:
    driver.quit()