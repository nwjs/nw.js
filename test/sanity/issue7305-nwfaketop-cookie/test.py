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
try:
    os.remove('svrlog.txt')
except:
    pass
try:
    os.remove('port.txt')
except:
    pass
server = subprocess.Popen(['python3', '../http-server-node.py', 'server.js'])
while not os.path.exists('port.txt'):
    time.sleep(1)
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
try:
    print(driver.current_url)
    driver.implicitly_wait(10)
    driver.switch_to.frame(driver.find_element_by_tag_name('iframe'))
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert result == 'same-site-cookie=foo; cross-site-cookie=bar; no-samesite-cookie=nee'
    driver.refresh()
    driver.switch_to.frame(driver.find_element_by_tag_name('iframe'))
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert result == 'same-site-cookie=foo; cross-site-cookie=bar; no-samesite-cookie=nee'
    f = open('svrlog.txt', 'r')
    svrlog = ''.join(f.readlines())
    f = open('expected.txt', 'r')
    expected = ''.join(f.readlines())
    assert svrlog == expected
finally:
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
    driver.quit()
