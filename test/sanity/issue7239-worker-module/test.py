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
    driver.find_element_by_tag_name('button').click()
    wait_for_element_id_content(driver, 'result', '456')
    time.sleep(1)
    driver.find_element_by_tag_name('button').click()
    wait_for_element_id_content(driver, 'result', '456')
finally:
    driver.quit()
