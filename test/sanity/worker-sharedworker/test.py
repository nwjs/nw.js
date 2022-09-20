import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_nw_argument("--enable-node-worker")

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    driver.find_element_by_id('start').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert('success' in result)
finally:
    driver.quit()
