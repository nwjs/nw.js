import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print(driver.current_url)
    driver.find_element_by_tag_name('button').click()
    print('wait for window open')
    wait_window_handles(driver, 2)
    print(driver.window_handles)
    print('switch_to')
    driver.switch_to.window('child')
    print('try close')
    driver.close()
    print('wait for window close')
    wait_window_handles(driver, 1, timeout=5)
    assert (len(driver.window_handles) == 1)
finally:
    driver.quit()
