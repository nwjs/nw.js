import time
import os
import shutil
import platform
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

if platform.system() != 'Linux':
    print('Skipped for non Linux platform')
    sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)

    result1 = wait_for_element_id(driver, 'socket1')
    print(result1)
    assert('Socket 1 open' in result1)

    result2 = wait_for_element_id(driver, 'socket2')
    print(result2)
    assert('Socket 2 open' in result2)

    result3 = wait_for_element_id(driver, 'socket3')
    print(result3)
    assert('Socket 3 open' in result3)

    result4 = wait_for_element_id(driver, 'socket4')
    print(result4)
    assert('Socket 4 open' in result4)
finally:
    driver.quit()
