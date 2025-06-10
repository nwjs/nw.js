import time
import os
import platform
import sys
if platform.system() != 'Darwin':
    print('Skipped for non Mac platform')
    sys.exit(0)
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    wait_window_handles(driver, 2)
    handles = driver.window_handles
    print(handles)
    driver.switch_to.window(handles[1])
    output = driver.current_url
    print(output)
    print('waiting for crash')
    time.sleep(5)
    assert driver.title == 'Google'
    assert 'https://www.google.com/' in output
    print('There is no crash')
finally:
    driver.quit()