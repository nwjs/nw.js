import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
time.sleep(1)
try:
    print driver.current_url
    driver.implicitly_wait(10)
    wait_window_handles(driver, 2)
    popup_window = None
    main_window = None
    for handle in driver.window_handles:
      driver.switch_to_window(handle)
      print driver.title
      if 'new popup' in driver.title:
        popup_window = handle
      else:
        main_window = handle
    assert(popup_window)
    assert(main_window)
    print 'switch to opened window'
    driver.switch_to_window(popup_window)
    result = wait_for_element_id(driver, 'yellow')
    print 'window size: %s' % result
    assert('200, 300' in result)
    driver.switch_to_window(main_window)
    driver.find_element_by_id('btn_resizeto').click()
    driver.switch_to_window(popup_window)
    result = driver.find_element_by_id('yellow').get_attribute('innerHTML')
    print 'window size: %s' % result
    assert('180, 180' in result)

finally:
    driver.quit()
