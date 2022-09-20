import time
import os
import shutil
import subprocess
import platform
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

os.chdir(testdir)

install_native_modules()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert("456" == result)
finally:
    driver.quit()
