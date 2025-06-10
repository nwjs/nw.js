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
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
os.chdir(testdir)
install_native_modules()
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    result = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
    print(result)
    assert '456' == result
finally:
    driver.quit()