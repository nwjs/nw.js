import time
import os
import sys
import platform

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

if platform.system() != 'Linux':
    sys.exit(0)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert("Permission denied" not in result)
finally:
    driver.quit()
