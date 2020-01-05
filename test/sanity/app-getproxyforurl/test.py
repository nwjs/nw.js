import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

test_dir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument("nwapp=" + test_dir)

port = str(utils.free_port())

pkgjson = '''
{
  "name": "app-getproxyforurl",
  "main": "index.html",
  "bg-script": "bg.js",
  "port": "%s"
}
''' % port

with open(os.path.join(test_dir, 'package.json'), 'w') as bg:
  bg.write(pkgjson)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url

    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("success" in result.get_attribute('innerHTML'))

    wait_window_handles(driver, 1)
    switch_to_app(driver)
    driver.find_element_by_id('pac').click()
    wait_window_handles(driver, 1)
    result2 = driver.find_element_by_id('result2')
    assert("success" in result2.get_attribute('innerHTML'))
    wait_window_handles(driver, 1)
    result3 = driver.find_element_by_id('result3')
    assert("success" in result3.get_attribute('innerHTML'))
finally:
    driver.quit()
