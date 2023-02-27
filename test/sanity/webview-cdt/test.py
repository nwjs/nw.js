import time
import os
import urllib.parse, urllib.request, urllib.parse, urllib.error
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

def path2url(path):
        return urllib.parse.urljoin(
                  'file:', urllib.request.pathname2url(path))

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_experimental_option("windowTypes", ["webview"])

capabilities = {"pageLoadStrategy": "none"}

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(10)
try:
    wait_for_element_id(driver, 'testbtn')
    driver.find_element_by_id('testbtn').click()
    timeout = 15
    found = False
    while timeout > 0 and not found:
        try:
            for handle in driver.window_handles:
                driver.switch_to.window(handle)
                if driver.current_url.startswith('devtools://'):
                    found = True
                    break
        except:
            pass
        timeout = timeout - 1
        time.sleep(1)
    assert(found)
finally:
    driver.quit()
