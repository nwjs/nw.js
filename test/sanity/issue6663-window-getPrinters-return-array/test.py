import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print(driver.current_url)
    print('wait for devtools open')
    wait_window_handles(driver, 2)
    print(driver.window_handles)
    print('switch to devtools')
    switch_to_devtools(driver)
    print('click Console panel')
    devtools_click_tab(driver, 'console')
    print('check if win.getPrinters receive an array of JSON objects')
    elems = driver.find_elements(By.CLASS_NAME, 'console-message-text')
    output = ''
    if len(elems) > 1:
        for i in range(len(elems)):
            if 'Array' in elems[i].get_attribute('innerHTML'):
                output = elems[i].get_attribute('innerHTML')
                break
    else:
        output = elems[0].get_attribute('innerHTML')
    print(output)
    assert 'Array' in output or '[]' in output
finally:
    driver.quit()