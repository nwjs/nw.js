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
    print driver.current_url
    wait_for_element_id_content(driver, 'result', 'success')
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("success" in result.get_attribute('innerHTML'))
finally:
    driver.quit()
