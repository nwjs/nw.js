import time
import os
import platform
import sys

if platform.system() != 'Darwin':
    print 'Skipped for non Mac platform'
    sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    driver.find_element_by_id('winclose').click()
    assert(driver.find_element_by_id('result') is None)
finally:
    driver.quit()
