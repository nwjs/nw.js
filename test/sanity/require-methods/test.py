import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('require-resolve').get_attribute('innerHTML')
    print(result)
    assert('success' in result)
    result = driver.find_element_by_id('require-cache').get_attribute('innerHTML')
    print(result)
    assert('success' in result)
    result = driver.find_element_by_id('require-extensions').get_attribute('innerHTML')
    print(result)
    assert('success' in result)
finally:
    driver.quit()
