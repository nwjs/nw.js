import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
import time
import os
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port, server = start_http_server()
manifest = open('package.json', 'w')
manifest.write('\n{\n  "name":"test-node-remote",\n  "node-remote":"<all_urls>",\n  "main":"http://127.0.0.1:%s/index.html"\n}\n' % port)
manifest.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(10)
try:
    print(driver.current_url)
    result = driver.find_element(By.ID, 'result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    stop_http_server(server)
    driver.quit()
