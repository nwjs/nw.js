import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_argument("mixed-context")

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port], shell=True)

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    driver.find_element_by_id('open-win').click()
    wait_window_handles(driver, 2)
    driver.find_element_by_id('close-win').click()
    wait_window_handles(driver, 1)
    driver.find_element_by_id('test-request').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert("success" in result)
finally:
    server.terminate()
    driver.quit()

