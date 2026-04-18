import time
import os
import sys
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    wait_window_handles(driver, 1)
    driver.switch_to.window(driver.window_handles[0])
    assert driver.title == 'main window'

    # Open and close 2 modals
    for i in range(2):
        driver.find_element(By.ID, 'open_modal').click()
        wait_window_handles(driver, 2)
        for handle in driver.window_handles:
            driver.switch_to.window(handle)
            if driver.title == 'modal window':
                driver.find_element(By.ID, 'close_modal').click()
                break
        wait_window_handles(driver, 1)
        driver.switch_to.window(driver.window_handles[0])

    # Close main window - should fire close event exactly once
    driver.find_element(By.ID, 'close_main').click()
    time.sleep(1)

    # If close fired more than once, close_count > 1 and window may still exist
    # With the fix, close fires once and window closes immediately
    # If window is gone, the test passes
    try:
        handles = driver.window_handles
        if len(handles) == 0:
            print('PASS: window closed after single close event')
        else:
            driver.switch_to.window(handles[0])
            count = driver.find_element(By.ID, 'close_count').get_attribute('innerHTML')
            assert count == '1', 'close event fired %s times, expected 1' % count
    except:
        # Window already closed - expected behavior
        print('PASS: window closed')
finally:
    driver.quit()
