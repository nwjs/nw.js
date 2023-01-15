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
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

try:
    os.remove('svrlog.txt')
except:
    pass

port = str(utils.free_port())
server = subprocess.Popen(['python3', 'https-server.py', port])

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    driver.switch_to.frame(driver.find_element_by_tag_name("iframe"))
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert(result == 'cross-site-cookie=bar')
    driver.refresh()
    driver.switch_to.frame(driver.find_element_by_tag_name("iframe"))
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert(result == 'cross-site-cookie=bar')

    f = open('svrlog.txt', 'r')
    svrlog = ''.join(f.readlines())
    f = open('expected.txt', 'r')
    expected = ''.join(f.readlines())
    assert (svrlog == expected)

finally:
    server.terminate()
    driver.quit()
