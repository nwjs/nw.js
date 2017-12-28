import time
import os
import shutil
import platform
import sys

if platform.system() != 'Linux':
    print 'Skipped for non Linux platform'
    sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    print driver.current_url

    result1 = driver.find_element_by_id('socket1').get_attribute('innerText')
    assert('Socket 1 open' in result1)

    result2 = driver.find_element_by_id('socket2').get_attribute('innerText')
    assert('Socket 2 open' in result2)

    result3 = driver.find_element_by_id('socket3').get_attribute('innerText')
    assert('Socket 3 open' in result3)

    result4 = driver.find_element_by_id('socket4').get_attribute('innerText')
    assert('Socket 4 open' in result4)
finally:
    driver.quit()
