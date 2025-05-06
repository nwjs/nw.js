import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
import subprocess
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
manifest.write('\n{\n  "name":"test-node-remote",\n  "main":"http://localhost:%s/index.html"\n}\n' % port)
manifest.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'result', 'success')
    print(result)
finally:
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
    driver.quit()