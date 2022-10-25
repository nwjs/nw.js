import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_experimental_option("windowTypes", ["webview"])

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

try:
    os.remove('port.txt')
except:
    pass
server = subprocess.Popen(['python', '../http-server-node.py'])

while not os.path.exists('port.txt') :
    time.sleep(1)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    wait_switch_window_url(driver, '1.html')
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert('success' in result)
finally:
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
    driver.quit()
