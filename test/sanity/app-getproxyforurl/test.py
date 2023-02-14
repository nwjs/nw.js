import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.common.utils import free_port

test_dir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument("nwapp=" + test_dir)
port = str(free_port())
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

driver = webdriver.Chrome(service=Service(executable_path=os.environ['CHROMEDRIVER']), options=chrome_options)
driver.implicitly_wait(5)
try:
    result = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
    assert("success" in result)
    wait_window_handles(driver, 1)
    switch_to_app(driver)
    driver.find_element(By.ID, 'pac').click()
    wait_window_handles(driver, 1)
    result2 = driver.find_element(By.ID, 'result2').get_attribute('innerHTML')
    assert("success" in result2)
    wait_window_handles(driver, 1)
    result3 = driver.find_element(By.ID, 'result3').get_attribute('innerHTML')
    assert("success" in result3)
finally:
    driver.quit()
