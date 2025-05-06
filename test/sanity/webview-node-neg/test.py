import time
import os
import urllib.parse, urllib.request, urllib.parse, urllib.error
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

def path2url(path):
    return urllib.parse.urljoin('file:', urllib.request.pathname2url(path))
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
chrome_options = Options()
chrome_options.add_argument('nwapp=' + testdir)
chrome_options.add_experimental_option('windowTypes', ['webview'])
capabilities = {'pageLoadStrategy': 'none'}
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log', additional_capabilities=capabilities)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    wait_switch_window_url(driver, 'webview.html')
    result = driver.find_element_by_id('ret').get_attribute('innerHTML')
    print(result)
    assert 'version = undefined' in result
finally:
    driver.quit()