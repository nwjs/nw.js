import time
import os
import subprocess
import platform
import sys
if platform.system() == 'Linux':
    print('Skipped for Linux platform')
    sys.exit(0)
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port])
manifest = open('package.json', 'w')
manifest.write('\n{\n  "name":"issue4579-capture-remote",\n  "node-remote":"<all_urls>",\n  "main":"http://localhost:%s/index.html"\n}\n' % port)
manifest.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    driver.find_element_by_id('start-share').click()
    result = driver.find_element_by_id('result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    server.terminate()
    driver.quit()
