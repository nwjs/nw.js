import time
import os
import shutil
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

datadir = os.path.join(testdir, "extract")
if os.path.isdir(datadir) :
  shutil.rmtree(datadir)

os.chdir(testdir)

install_node_modules()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print(driver.current_url)
    time.sleep(5)
    result = driver.find_element_by_id('result')
    print(result.get_attribute('innerHTML'))
    assert("success" in result.get_attribute('innerHTML'))
finally:
    driver.quit()
