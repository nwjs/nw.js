import time
import os
import shutil
import subprocess
import platform
import sys
from subprocess import Popen, PIPE
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    timeout = 5
    while timeout > 0:
        ret = driver.find_element_by_id('result').get_attribute('innerHTML')
        print('result: ', ret)
        if 'waiting' not in ret:
            break
        time.sleep(1)
        timeout = timeout - 1
    assert 'waiting' not in ret
finally:
    driver.quit()