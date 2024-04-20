import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_argument("user-data-dir=" + os.path.join(testdir, 'userdata'))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
try:
    print((driver.current_url))
    second_instance = subprocess.Popen(['python3', 'second_instance.py'])
    result = wait_for_element_id_content(driver, 'result', 'success')
    print(result)
finally:
    driver.quit()
    #second_instance.kill()
