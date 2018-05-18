import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print driver.current_url
    print "wait for 2 windows open"
    wait_window_handles(driver, 2)
    print driver.window_handles
    time.sleep(1)
    print "close one window"
    wait_window_handles(driver, 1)
    print driver.window_handles
    result = wait_for_element_id(driver, "result")
    assert("success" in result)
finally:
    driver.quit()
