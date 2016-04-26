import time
import os
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("success" in result.get_attribute('innerHTML'))
finally:
    driver.quit()

