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
capabilities = {'pageLoadStrategy': 'none'}
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log', additional_capabilities=capabilities)
driver.implicitly_wait(5)
try:
    wait_switch_window_url(driver, 'index.html')
    # devicePixelRatio includes the system device scale factor, so capture
    # the baseline value of the index window before any zoom is applied and
    # compare against it instead of hardcoded values (which only hold at
    # device scale factor 1)
    base_dpr = float(driver.execute_script('return window.devicePixelRatio'))
    driver.find_element(By.ID, 'create-popup').click()
    wait_for_element_id_content(driver, 'result', 'created')
    wait_window_handles(driver, 2)
    wait_switch_window_url(driver, 'popup.html')
    zoom2 = 'zoom level'
    deadline = time.time() + 30
    while zoom2 == 'zoom level':
        zoom2 = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
        if zoom2 == 'zoom level':
            if time.time() > deadline:
                raise Exception('popup did not report its zoom level within 30s')
            time.sleep(1)
    print('zoom2: ', zoom2)
    wait_switch_window_name(driver, 'index')
    driver.find_element(By.ID, 'get-zoom').click()
    zoom1 = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
    print('zoom1: ', zoom1)
    # the popup sets zoom level 2 (zoom factor 1.2^2 = 1.44); the index
    # window must not inherit it and the popup's ratio must change
    assert abs(float(zoom1) - base_dpr) < 0.01, 'index window zoom changed: %s (base %s)' % (zoom1, base_dpr)
    zoom2f = float(zoom2)
    assert abs(zoom2f / base_dpr - 1.44) < 0.1, 'popup zoom not applied: %s (base %s)' % (zoom2, base_dpr)
finally:
    driver.quit()