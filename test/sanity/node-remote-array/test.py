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
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port, server = start_http_server()
manifest = open('package.json', 'w')
manifest.write('\n{\n  "name":"test-node-remote",\n  "node-remote":["<all_urls>"],\n  "main":"http://localhost:%s/index.html"\n}\n' % port)
manifest.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    result = wait_for_element_id(driver, 'result')
    print(result)
    assert 'success' in result
finally:
    stop_http_server(server)
    driver.quit()
