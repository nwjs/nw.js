import time
import os
import platform
import sys
if platform.system() != 'Linux':
    print('Skipped for non Linux platform')
    sys.exit(0)
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
node_module = os.path.join(testdir, 'node_modules')
os.chdir(testdir)
install_native_modules()
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print(driver.current_url)
    print('waiting for crash')
    wait_for_element_id(driver, 'iframe_a')
    time.sleep(5)
    print('There is no crash')
finally:
    driver.quit()