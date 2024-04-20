import time
import os
import subprocess
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port], shell=True)

manifest = open('package.json', 'w')
manifest.write('''
{
  "name":"test-node-remote",
  "node-remote":["<all_urls>"],
  "main":"http://localhost:%s/index.html"
}
''' % (port))
manifest.close()

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
try:
    print(driver.current_url)
    result = wait_for_element_id(driver, 'result')
    print(result)
    assert("success" in result)
finally:
    server.terminate()
    driver.quit()

