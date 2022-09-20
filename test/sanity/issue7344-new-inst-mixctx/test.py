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
    driver.find_element_by_id('spawnRender').click()
    print('wait for window open')
    wait_window_handles(driver, 2)
    print(driver.window_handles)
    print('switch to 1st window')
    driver.switch_to.window(driver.window_handles[-1])
    result1 = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result1)
    assert('undefined' in result1) 
finally:
    driver.quit()
