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
  "name": "cookie-lost-remote-page",
  "main": "main.js",
  "node-remote": "http://localhost/*",
  "port": "%s"
}
''' % port

with open(os.path.join(test_dir, 'package.json'), 'w') as bg:
  bg.write(pkgjson)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    wait_window_handles(driver, 1)
    switch_to_app(driver)
    print(driver.current_url)
    driver.find_element_by_id('open-cdt').click()
    time.sleep(1)
    driver.find_element_by_id('close-cdt').click()
    time.sleep(1)
    driver.find_element_by_id('get-cookie').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert('sid=1' in result)
finally:
    driver.quit()
