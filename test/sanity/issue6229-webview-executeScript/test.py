import time
import os
import platform
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    print('wait for devtools open')
    wait_window_handles(driver, 3)
    handles = driver.window_handles
    print(handles)
    driver.switch_to.window(handles[0])
    print('click button to execute code')
    driver.find_element(By.TAG_NAME, 'button').click()
    print('get all keys in window')
    result = driver.find_element(By.CLASS_NAME, 'log').get_attribute('innerText')
    assert 'postMessage' in result
finally:
    driver.quit()