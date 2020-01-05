import time
import os
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    print driver.current_url
    driver.find_element_by_tag_name('button').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
    driver.find_element_by_tag_name('a').click()
    wait_for_element_tag(driver, 'button')
    driver.find_element_by_tag_name('button').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
finally:
    driver.quit()
