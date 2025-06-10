import time
import os
import subprocess
import sys
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port])
html = open('index.html', 'w')
html.write('\n<!DOCTYPE html>\n<html lang="en">\n  <head>\n    <meta charset="utf8">\n    <title>iframe-remote negative test</title>\n  </head>\n  <body>\n    <iframe src="http://localhost:%s/test.html"></iframe>\n    <script>\n      document.write(\'<h1 id="res1">Node is \' + (typeof nw === \'undefined\' ? \'DISABLED\': \'ENABLED\') + \'</h1>\');\n    </script>\n  </body>\n</html>\n' % port)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element(By.ID, 'res1').get_attribute('innerHTML')
    print(result)
    assert 'ENABLED' in result
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
    assert 'DISABLED' in ret
finally:
    server.terminate()
    driver.quit()