import time
import os
import shutil
import platform
import subprocess
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port])

chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)

#To do removed before merging
os.environ['CHROMEDRIVER'] = "$HOME/.nvm/versions/node/v18.13.0/bin/chromedriver"

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    driver.get('http://localhost:%s/index.html' % port)
    result = driver.get_element_by_id("file-input")
    assert("success" in result.get_attribute('innerHTML'))
except:
    print("dom-input-file-append")
    print(sys.exc_info()[0])
    print(sys.exc_info()[1])
    print(sys.exc_info()[2])
finally:
    server.terminate()
    driver.quit()
