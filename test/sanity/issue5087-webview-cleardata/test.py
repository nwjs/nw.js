import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_experimental_option('windowTypes', ['webview'])
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
svrprog = os.path.abspath(os.path.join(os.path.dirname(testdir), 'http-server-node.py'))
port = str(utils.free_port())
server = subprocess.Popen(['python', svrprog, port])
tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()
html = open('index.html', 'w')
html.write(content)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    wait_window_handles(driver, 2)
    wait_switch_window_name(driver, 'index')
    print(driver.current_url)
    driver.find_element(By.ID, 'clear').click()
    result = wait_for_element_id_content(driver, 'result', 'success')
    print(result)
    driver.find_element(By.ID, 'clear').click()
    result = wait_for_element_id_content(driver, 'result', 'success')
    print(result)
finally:
    driver.quit()
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
