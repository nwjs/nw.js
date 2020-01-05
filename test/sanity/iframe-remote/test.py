import time
import os
import subprocess
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', 'http-server.py', port])

html = open('index.html', 'w')
html.write('''
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf8">
    <title>iframe-remote negative test</title>
  </head>
  <body>
    <iframe src="http://localhost:%s/test.html"></iframe>
    <script>
      document.write('<h1 id="res1">Node is ' + (typeof nw === 'undefined' ? 'DISABLED': 'ENABLED') + '</h1>');
    </script>
  </body>
</html>
''' % (port))
    
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
try:
    print driver.current_url
    result = driver.find_element_by_id('res1').get_attribute('innerHTML')
    print result
    assert("ENABLED" in result)
    timeout = 10
    ret = ''
    elem_id = 'res2'
    while timeout > 0:
        try:
            driver.switch_to_frame(driver.find_element_by_tag_name("iframe"))
            ret = driver.find_element_by_id(elem_id).get_attribute('innerHTML')
            break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
             raise Exception('Timeout when waiting for element' + elem_id)
    print ret
    assert("ENABLED" in ret)
finally:
    server.terminate()
    driver.quit()

