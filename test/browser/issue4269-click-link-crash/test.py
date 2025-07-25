import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port])
tpl = open('inspect.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()
html = open('inspect.html', 'w')
html.write(content)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print(driver.current_url)
    driver.find_element(By.ID, 'test').click()
    wait_for_element_id_content(driver, 'progress', 'loaded')
    print(driver.window_handles)
    handles = driver.window_handles
    driver.find_element(By.ID, 'opendev').click()
    print('wait for devtools window')
    wait_window_handles(driver, 3)
    print(driver.window_handles)
    devwin = list(set(driver.window_handles) - set(handles))[0]
    print('switch to devtools window ' + devwin)
    driver.switch_to.window(devwin)
    devtools_click_tab(driver, 'console')
    wait_for_execute_script(driver, 'document.querySelector(".console-message-text .devtools-link").click()')
    wait_window_handles(driver, 4)
    driver.switch_to.window(driver.window_handles[3])
    print(driver.current_url)
    assert 'http://localhost:%s/inspect.html' % port in driver.current_url
finally:
    server.terminate()
    driver.quit()