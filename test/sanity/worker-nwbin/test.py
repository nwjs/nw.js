import time
import os
import shutil
import subprocess
import platform
from subprocess import Popen, PIPE
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
binfile = os.path.join(testdir, 'nw.bin')
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwjc.exe' if os.name == 'nt' else 'nwjc')
os.chdir(testdir)
try:
    os.remove(binfile)
except:
    pass
assert False == os.path.isfile(binfile)
subprocess.call([nwjc, 'mytest.js', 'nw.bin'])
assert os.path.isfile(binfile)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'result', '840')
    print(result)
    assert '840' == result
finally:
    driver.quit()