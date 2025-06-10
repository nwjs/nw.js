import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    switch_to_app(driver)
    print(driver.current_url)
    print('wait for devtools open')
    wait_window_handles(driver, 2)
    print(driver.window_handles)
    print('switch to devtools')
    switch_to_devtools(driver)
    print('click Console panel')
    devtools_click_tab(driver, 'console')
    print('check if there is warning message in console panel')
    elems = driver.find_elements(By.CLASS_NAME, 'console-message-text')
    output = ''
    if len(elems) > 2:
        for i in range(len(elems)):
            if 'MediaElementAudioSource' in elems[i].get_attribute('innerHTML'):
                output = elems[i].get_attribute('innerHTML')
                break
    elif len(elems) == 1:
        output = elems[0].get_attribute('innerHTML')
    else:
        output = ''
    print(output)
    assert 'MediaElementAudioSource' not in output
    assert output is ''
finally:
    driver.quit()