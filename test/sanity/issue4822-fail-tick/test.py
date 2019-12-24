import time
import os
import sys
from selenium.webdriver.common.by import By

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
    driver.find_element(By.XPATH, '//button[text()="fail"]').click()
    driver.find_element(By.XPATH, '//button[text()="process.nextTick"]').click()
    result = driver.find_element_by_id('res1').get_attribute('innerHTML')
    print result
    assert("success" in result)
finally:
    driver.quit()
