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
server = subprocess.Popen(['python', 'https-server.py', port])

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
time.sleep(1)
try:
    print driver.current_url
    driver.implicitly_wait(10)
    driver.find_element_by_id('winopen').click()
    print 'wait for window open'
    wait_window_handles(driver, 2)
    print 'switch to new window'
    driver.switch_to_window(driver.window_handles[-1]) # switch to new window
    driver.find_element_by_id('showopener').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert("success" in result)
finally:
    server.terminate()
    driver.quit()
