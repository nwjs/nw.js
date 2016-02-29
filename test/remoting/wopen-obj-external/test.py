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

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', 'http-server.py', port])

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
time.sleep(1)
try:
    print driver.current_url
    driver.find_element_by_id('open-win').click()
    wait_window_handles(driver, 2)
    driver.switch_to_window(driver.window_handles[-1])
    driver.find_element_by_tag_name('button').click() #click close button
    driver.switch_to_window(driver.window_handles[0])
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)

finally:
    server.terminate()
    driver.quit()

