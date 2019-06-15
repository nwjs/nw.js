import time
import os
import urlparse, urllib
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

def path2url(path):
        return urlparse.urljoin(
                  'file:', urllib.pathname2url(path))

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_experimental_option("windowTypes", ["webview"])

capabilities = {"pageLoadStrategy": "none"}

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    #driver.find_element_by_id('testbtn').click()
    timeout = 10
    found = False
    while timeout > 0 and not found:
        for handle in driver.window_handles:
            driver.switch_to_window(handle)
            if driver.current_url.startswith('devtools://'):
                found = True
                break
        timeout = timeout - 1
        time.sleep(1)
    assert(found)
    driver.execute_script('UI.inspectorView._tabbedPane.selectTab(UI.inspectorView._tabbedPane._tabs[9]._id)')
    title = driver.execute_script('return UI.inspectorView._tabbedPane._tabs[9]._title')
    print "title:", title
    assert('custom' in title)
finally:
    driver.quit()
