import time
import os
import urllib.parse, urllib.request, urllib.parse, urllib.error
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

svrprog = os.path.abspath(os.path.join(os.path.dirname(testdir), 'http-server-node.py'))

port = str(utils.free_port())
server = subprocess.Popen(['python', svrprog, port])

tpl = open('window.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('window.html', 'w')
html.write(content)
html.close()

chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_experimental_option("windowTypes", ["webview"])

capabilities = {"pageLoadStrategy": "none"}

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    wait_switch_window_name(driver, 'webview0')
    result = driver.find_element_by_id('ret').get_attribute('innerHTML')
    print(result)
    assert('version = v' in result)
finally:
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
    driver.quit()
