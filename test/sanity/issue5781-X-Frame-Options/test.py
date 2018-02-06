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

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    print driver.current_url
    print 'loading http server in iframe element'
    wait_for_element_tag(driver, "iframe")
    assert(driver.find_element_by_tag_name("iframe") is not None)
    driver.switch_to_frame(driver.find_element_by_tag_name("iframe"))
    res2 = wait_for_element_id(driver, 'res2')
    assert( "Node is DISABLED" in res2)
    print 'page iframe is loaded'
finally:
    server.terminate()
    driver.quit()
