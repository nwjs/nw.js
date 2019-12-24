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
driver.implicitly_wait(5)
try:
    print driver.current_url
    result = driver.find_element_by_id('inject_end').get_attribute('innerHTML')
    print 'inject_js_end: %s' % result
    assert('success' in result)
    elems = driver.find_elements_by_tag_name('h1')
    assert(len(elems) == 1)
    driver.switch_to_frame(driver.find_element_by_tag_name("iframe"))
    result = driver.find_element_by_id('inject_end').get_attribute('innerHTML')
    print 'inject_js_end in iframe: %s' % result
    assert('success' in result)
    elems = driver.find_elements_by_tag_name('h1')
    assert(len(elems) == 1)
finally:
    driver.quit()
