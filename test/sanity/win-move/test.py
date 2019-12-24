import time
import os
import platform
import sys

if platform.system() == 'Linux':
  print 'Skiping win-move because of upstream issue. See nwjs/nw.js#4217.'
  sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(10)
try:
    print driver.current_url
    driver.find_element_by_id('moveto').click()
    result = driver.find_element_by_id('moveto-result').get_attribute('innerHTML')
    print result
    assert('success' in result)

    driver.find_element_by_id('moveby').click()
    result = driver.find_element_by_id('moveby-result').get_attribute('innerHTML')
    print result
    assert('success' in result)
finally:
    driver.quit()
