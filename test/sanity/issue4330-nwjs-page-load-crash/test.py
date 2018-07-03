import time
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options)
driver.implicitly_wait(2)

try:
    print driver.current_url
    print "wait 5 minutes to crash"
    time.sleep(5)
    assert("NW.js" in driver.title)
    print "There is no crash"
finally:
    driver.quit()
