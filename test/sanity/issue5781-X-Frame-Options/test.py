import time
import os
import sys
import subprocess

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', 'http-server.py', port])

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
try:
    print driver.current_url
    print 'loading http server in iframe element'
    wait_for_element_id(driver, "iframe_a")
    driver.switch_to_frame("iframe_a")
    ret = ''
    timeout = 10
    while timeout > 0:
        try:
            ret = driver.find_element_by_id('res2').get_attribute('innerHTML')
            break
        except selenium.common.exceptions.NoSuchElementException:
            driver.switch_to_default_content()
            driver.switch_to_frame("iframe_a")
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
             raise Exception('Timeout when waiting for element res2')
    assert( "Node is DISABLED" in ret)
    print 'page iframe is loaded'
finally:
    server.terminate()
    driver.quit()
