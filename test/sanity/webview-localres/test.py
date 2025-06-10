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
htmlfile = os.path.join(testdir, '1.html')
localurl = path2url(testdir)
tpl = open('index.tpl', 'r')
if sys.platform == 'win32':
    htmlfile = htmlfile.replace('\\', '\\\\')
content = tpl.read().replace('{localfile}', htmlfile)
content = content.replace('{localdirurl}', localurl)
tpl.close()
html = open('index.html', 'w')
html.write(content)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log', additional_capabilities=capabilities)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    wait_switch_window_name(driver, 'webview1')
    result = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
    print(result)
    assert 'success' in result
finally:
    driver.quit()