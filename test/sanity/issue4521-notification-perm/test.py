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
    print(result)
    assert expected in result
    if should_close_window == True:
        wait_window_handles(driver, 2)
        driver.switch_to.window(driver.window_handles[1])
        driver.close()
        driver.switch_to.window(driver.window_handles[0])
        wait_window_handles(driver, 1)
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_argument('skip-reopen-app-devtools')
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
try:
    os.remove('port.txt')
except:
    pass
server = subprocess.Popen(['python', '../http-server-node.py', 'server.js'])
while not os.path.exists('port.txt'):
    time.sleep(1)
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    test_perm(driver, 'show-perm', 'perm')
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True)
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote')
    test_perm(driver, 'clear-results', 'output', expected='cleared')
    test_perm(driver, 'set-perm-block', 'set-perm-block-result', expected='success')
    test_perm(driver, 'show-perm', 'perm')
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True, expected='denied')
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote', expected='denied')
    test_perm(driver, 'clear-results', 'output', expected='cleared')
    test_perm(driver, 'set-perm-ask', 'set-perm-ask-result', expected='success')
    test_perm(driver, 'show-perm', 'perm')
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True, expected='default')
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote', expected='denied')
    test_perm(driver, 'clear-results', 'output', expected='cleared')
    test_perm(driver, 'set-perm-allow', 'set-perm-allow-result', expected='success')
    test_perm(driver, 'show-perm', 'perm')
    test_perm(driver, 'show-perm-open-inside', 'perm-open-inside', should_close_window=True)
    test_perm(driver, 'show-perm-open-remote', 'perm-open-remote', should_close_window=True)
    test_perm(driver, 'show-perm-frame-inside', 'perm-frame-inside')
    test_perm(driver, 'show-perm-frame-remote', 'perm-frame-remote')
finally:
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
    driver.quit()