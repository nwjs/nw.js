import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import time
import shutil
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

datadir = os.path.join(testdir, "extract")
if os.path.isdir(datadir) :
  shutil.rmtree(datadir)

os.chdir(testdir)

install_node_modules()

driver = get_configured_webdriver(chrome_options_instance=chrome_options)

try:
    print(driver.current_url)
    time.sleep(5)
    result = driver.find_element(By.ID, 'result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    driver.quit()
