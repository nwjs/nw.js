import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By

def click_and_assert(driver, id, exp_w, exp_h):
    driver.find_element(By.ID, id).click()
    wait_window_handles(driver, 2)
    print('switch to opened window')
    driver.switch_to.window(driver.window_handles[-1])
    driver.find_element(By.ID, 'getwinsize').click()
    result = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
    print('window size: %s' % result)
    expect = str(exp_w) + ',' + str(exp_h)
    expect2 = str(exp_w + 1) + ',' + str(exp_h + 1)
    assert expect in result or expect2 in result
    driver.close()
    wait_window_handles(driver, 1)
    driver.switch_to.window(driver.window_handles[0])
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    print('open new window with `window.open()`')
    click_and_assert(driver, 'winopen', 400, 300)
    print('open new window by `window.open()` with width and height')
    print('newly opended window should have size of 320,350')
    click_and_assert(driver, 'winopenwithsize', 320, 350)
    print('open new window with <a target="_blank">')
    click_and_assert(driver, 'linkopen', 400, 300)
    print('bind new-win-policy and newly opened window should have size of 388,300')
    driver.find_element(By.ID, 'bindnewwinpolicy').click()
    print('open new window with `window.open()` after new-win-policy')
    click_and_assert(driver, 'winopen', 388, 300)
    print('open new window with <a target="_blank"> after new-win-policy')
    click_and_assert(driver, 'linkopen', 388, 300)
finally:
    driver.quit()