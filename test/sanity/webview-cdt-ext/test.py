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
from selenium.webdriver.common.by import By
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
    timeout = 10
    found = False
    while timeout > 0 and (not found):
        for handle in driver.window_handles:
            driver.switch_to.window(handle)
            if driver.current_url.startswith('devtools://'):
                found = True
                break
        timeout = timeout - 1
        time.sleep(1)
    assert found
    timeout = 3
    found = False
    title = ''
    while timeout > 0 and (not found):
        try:
            driver.execute_script('UI.inspectorView.tabbedPane.selectTab(UI.inspectorView.tabbedPane.tabs[10].id)')
            title = driver.execute_script('return UI.inspectorView.tabbedPane.tabs[10].title')
        except:
            pass
        print('title:', title)
        found = 'custom' in title
        time.sleep(1)
        timeout = timeout - 1
    print('title:', title)
    assert 'custom' in title
finally:
    driver.quit()