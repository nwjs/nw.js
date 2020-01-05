import time
import os
import subprocess
import platform
import sys

if platform.system() == 'Linux':
    print 'Skipped for Linux platform'
    sys.exit(0)


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

manifest = open('package.json', 'w')
manifest.write('''
{
  "name":"issue4579-capture-remote",
  "node-remote":"<all_urls>",
  "main":"http://localhost:%s/index.html"
}
''' % (port))
manifest.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    print driver.current_url
    driver.find_element_by_id('start-share').click()
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("success" in result.get_attribute('innerHTML'))
finally:
    server.terminate()
    driver.quit()
