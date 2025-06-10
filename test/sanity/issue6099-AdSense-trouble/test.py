import time
import os
import sys
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
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    print('Click frame button')
    driver.find_element(By.ID, 'refresh').click()
    output = driver.find_element(By.ID, 'frameInfo').get_attribute('innerHTML')
    assert 'Iframe top accessible: true' in output
    print('Switch to iframe')
    driver.switch_to.frame(driver.find_element(By.TAG_NAME, 'iframe'))
    print('Click iframe button')
    driver.find_element(By.ID, 'refresh').click()
    result = driver.find_element(By.ID, 'iframeInfo').get_attribute('innerHTML')
    assert 'Top accessible: true' in result
    print('document.querySelector("iframe").contentWindow.top is works')
finally:
    driver.quit()
    server.terminate()