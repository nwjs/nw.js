import time
import os
import sys
import subprocess

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_experimental_option("windowTypes", ["webview"])

capabilities = {"pageLoadStrategy": "none"}

port = str(utils.free_port())
server_file = os.path.join(testdir, "../", "http-server.py")
server = subprocess.Popen(['python', server_file, port])

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    print 'wait for window open'
    print driver.window_handles
    wait_window_handles(driver, 3)
    print 'switch to webview window'
    wait_switch_window_name(driver, 'webview0')
    res = wait_for_element_id(driver, "result")
    print res
    assert("in sandbox" in res)
finally:
    server.terminate()
    driver.quit()
