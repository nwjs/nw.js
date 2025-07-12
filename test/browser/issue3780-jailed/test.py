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
    print(driver.current_url)
    time.sleep(1)
    print('click button to show jailed devtools')
    driver.find_element(By.ID, 'showdevtools').click()
    print('wait for devtools open')
    wait_window_handles(driver, 2)
    print('switch to devtools')
    switch_to_devtools(driver)
    print('click Console panel')
    devtools_click_tab(driver, 'console')
    wait_for_element_id(driver, 'console-prompt')
    print('send_keys "location.pathname<enter>"')
    devtools_type_in_console(driver, 'location.')
    time.sleep(1)
    devtools_type_in_console(driver, 'pathname')
    devtools_type_in_console(driver, '\n')
    timeout = 10
    while timeout > 0:
        try:
            pathname = driver.find_element(By.CSS_SELECTOR, '.console-user-command-result .console-message-text .object-value-string').get_attribute('textContent')
            print(pathname)
            assert '/child.html' in pathname
            break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for result')
finally:
    driver.quit()
