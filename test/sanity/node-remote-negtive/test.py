import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', 'http-server.py', port])

manifest = open('package.json', 'w')
manifest.write('''
{
  "name":"test-node-remote",
  "main":"http://localhost:%s/index.html"
}
''' % (port))
manifest.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
try:
    print driver.current_url
    time.sleep(1)
    result = wait_for_element_id_content(driver, 'result', 'success')
    print result
finally:
    server.terminate()
    driver.quit()

