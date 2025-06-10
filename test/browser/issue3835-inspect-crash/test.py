import time
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
    wait_for_element_id(driver, 'console-prompt')
    print('send_keys "chrome<enter>"')
    devtools_type_in_console(driver, 'chrome\n')
    time.sleep(2)
    driver.execute_script('document.querySelector(".console-object-preview").click()')
    time.sleep(1)
    expanded = driver.find_element(By.CSS_SELECTOR, '.console-view-object-properties-section.expanded')
    assert expanded is not None
finally:
    driver.quit()