import time
import os
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

fd = open('index.tpl', 'r')
html = fd.read().replace('{port}', port)
fd.close()

fd = open('index.html', 'w')
fd.write(html)
fd.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    driver.find_element_by_id('to-remote').click()
    print driver.current_url
    assert('http://localhost' in driver.current_url)
    assert('/index.html' in driver.current_url)
    driver.find_element_by_id('history-back').click()
    print driver.current_url
    assert('chrome-extension://' in driver.current_url)
    assert('/index.html' in driver.current_url)
finally:
    server.terminate()
    driver.quit()