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

tpl = open('inspect.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('inspect.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
time.sleep(1)
try:
    switch_to_app(driver)
    print driver.current_url
    driver.find_element_by_id('test').click()
    print 'wait for devtools window'
    wait_window_handles(driver, 3)
    print driver.window_handles
    print 'switch to devtools window'
    switch_to_devtools(driver)
    devtools_click_tab(driver, 'console')
    driver.find_element_by_css_selector('.console-message-text .devtools-link').click()
    wait_window_handles(driver, 4)
    driver.switch_to_window(driver.window_handles[3])
    print driver.current_url
    assert(('http://localhost:%s/inspect.html' % port) in driver.current_url)
finally:
    server.terminate()
    driver.quit()
