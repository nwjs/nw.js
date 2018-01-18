import time
import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    print 'waiting to load storeId in cookie'
    result = wait_for_element_tag(driver, "h2")
    print result
    assert(len(result) > 0)
    print 'storeId is loaded'
finally:
    driver.quit()
