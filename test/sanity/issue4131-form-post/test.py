import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    driver.implicitly_wait(10)
    print((driver.current_url))
    driver.execute_script('startServer(%s)' % utils.free_port())
    result = driver.find_element_by_id('server-started').get_attribute('innerHTML')
    print('server started')
    driver.find_element_by_id('submit').click()
    result = driver.find_element_by_id('method').get_attribute('innerHTML')
    print(result)
    assert('POST' in result)
    driver.find_element_by_id('submit').click()
    result = driver.find_element_by_id('method').get_attribute('innerHTML')
    print(result)
    assert('POST' in result)
    result = driver.find_element_by_id('data').get_attribute('innerHTML')
    print(result)
    assert('myvalue2' in result)
finally:
    driver.quit()
