import time
import os
import platform
import sys

if platform.system() == 'Linux':
  print 'Skiping issue5247-window-x-y because of upstream issue. See nwjs/nw.js#4217.'
  sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
time.sleep(1)
try:
    print driver.current_url
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert ("200,100" in result)
finally:
    driver.quit()
