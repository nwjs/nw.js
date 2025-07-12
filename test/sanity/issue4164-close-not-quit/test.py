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
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(10)
try:
    print(driver.current_url)
    wait_window_handles(driver, 2)
    switch_to_devtools(driver, None, True)
    driver.close()
    wait_window_handles(driver, 1)
    driver.switch_to.window(driver.window_handles[0])
    driver.close()
    time.sleep(3)
    assert no_live_process(driver)
finally:
    driver.quit()