import time
import os
import sys
import subprocess
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
os.chdir(testdir)
port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port])
tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()
html = open('index.html', 'w')
html.write(content)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    print('loading http server in iframe element')
    wait_for_element_id(driver, 'iframe_a')
    timeout = 10
    ret = ''
    elem_id = 'res2'
    while timeout > 0:
        try:
            driver.switch_to.frame(driver.find_element(By.TAG_NAME, 'iframe'))
            ret = driver.find_element(By.ID, elem_id).get_attribute('innerHTML')
            break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for element' + elem_id)
    print(ret)
    assert 'Node is' in ret
finally:
    server.terminate()
    driver.quit()