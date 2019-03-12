import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

def test_perm(driver, click_id, find_id, should_close_window=False, expected='granted'):
    driver.find_element_by_id(click_id).click()
    result = wait_for_element_id(driver, find_id)
    print result
    assert(expected in result)
    if should_close_window == True:
        wait_window_handles(driver, 2)
        driver.switch_to_window(driver.window_handles[1])
        driver.close()
        driver.switch_to_window(driver.window_handles[0])
        wait_window_handles(driver, 1)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_argument("skip-reopen-app-devtools")

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server_path = os.path.join(testdir, '../', 'http-server.py')
server = subprocess.Popen(['python', server_path, port])

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
    # 1. default 'granted' on all domains
    # perm for index.html
    test_perm(driver, 'show-perm', 'perm')

    # perm for local open window
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)

    # perm for remote open window
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True)

    # perm for local frame
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')

    # perm for remote frame
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote')

    # clear results
    test_perm(driver, 'clear-results', 'output', expected='cleared')

    # 2. block notification permission on localhost
    test_perm(driver, 'set-perm-block', 'set-perm-block-result', expected='success')

    # perm for index.html
    test_perm(driver, 'show-perm', 'perm')

    # perm for local open window
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)

    # perm for remote open window
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True, expected='denied')

    # perm for local frame
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')

    # perm for remote frame
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote', expected='denied')

    # clear results
    test_perm(driver, 'clear-results', 'output', expected='cleared')

    # 3. set notification permission to default on localhost
    test_perm(driver, 'set-perm-ask', 'set-perm-ask-result', expected='success')

    # perm for index.html
    test_perm(driver, 'show-perm', 'perm')

    # perm for local open window
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)

    # perm for remote open window
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True, expected='default')

    # perm for local frame
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')

    # perm for remote frame
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote', expected='denied')

    # clear results
    test_perm(driver, 'clear-results', 'output', expected='cleared')

    # 4. allow notification permission on localhost
    test_perm(driver, 'set-perm-allow', 'set-perm-allow-result', expected='success')

    # perm for index.html
    test_perm(driver, 'show-perm', 'perm')

    # perm for local open window
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)

    # perm for remote open window
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True)

    # perm for local frame
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')

    # perm for remote frame
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote')
finally:
    server.terminate()
    driver.quit()

