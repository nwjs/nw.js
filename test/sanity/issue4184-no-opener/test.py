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
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_argument('mixed-context')
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port = str(utils.free_port())
server = subprocess.Popen(['python3', 'https-server.py', port])
tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()
html = open('index.html', 'w')
html.write(content)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    driver.find_element_by_id('winopen').click()
    print('wait for window open')
    wait_window_handles(driver, 2)
    print('switch to new window')
    driver.switch_to.window(driver.window_handles[-1])
    wait_for_element_id(driver, 'showopener')
    driver.find_element_by_id('showopener').click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert 'success' in result
finally:
    server.terminate()
    driver.quit()