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
time.sleep(1)
try:
    print driver.current_url
    driver.find_element_by_id('spawnRender').click()
    driver.find_element_by_id('spawnRender').click()
    print 'wait for window open'
    wait_window_handles(driver, 3)
    print driver.window_handles
    print 'switch to 1st window'
    driver.switch_to_window(driver.window_handles[-2])
    result1 = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result1
    print 'switch to 2nd window'
    driver.switch_to_window(driver.window_handles[-1])
    result2 = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result2
    assert result1 != result2, "renders should not share same pid"
finally:
    driver.quit()
