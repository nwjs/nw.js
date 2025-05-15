import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
import time
import os
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_argument('mixed-context')
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

start_http_server()

driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    driver.find_element_by_id('winopen').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert 'success' in result
finally:
    server.terminate()
    driver.quit()
