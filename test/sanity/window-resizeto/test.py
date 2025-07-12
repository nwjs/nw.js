import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    wait_window_handles(driver, 2)
    popup_window = None
    main_window = None
    for handle in driver.window_handles:
        driver.switch_to.window(handle)
        print(driver.title)
        if 'new popup' in driver.title:
            popup_window = handle
        else:
            main_window = handle
    assert popup_window
    assert main_window
    print('switch to opened window')
    driver.switch_to.window(popup_window)
    result = wait_for_element_id(driver, 'yellow')
    print('window size: %s' % result)
    assert '200, 300' in result
    driver.switch_to.window(main_window)
    driver.find_element(By.ID, 'btn_resizeto').click()
    driver.switch_to.window(popup_window)
    result = wait_for_element_id_content(driver, 'yellow', '180, 180')
    print('window size: %s' % result)
finally:
    driver.quit()
