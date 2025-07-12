import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import shutil
import subprocess
import platform
from subprocess import Popen, PIPE
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
binfile = os.path.join(testdir, 'mytest.bin')
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwjc.exe' if os.name == 'nt' else 'nwjc')
os.chdir(testdir)
try:
    os.remove(binfile)
except:
    pass
assert False == os.path.isfile(binfile)
subprocess.call([nwjc, 'mytest.js', 'mytest.bin'])
assert os.path.isfile(binfile)
if platform.system() == 'Linux':
    proc = Popen(['strings', 'mytest.bin'], stdout=PIPE, stderr=PIPE)
    out, err = proc.communicate()
    ret = out.decode()
    print(ret)
    assert '42' not in ret
    assert 'foo' not in ret
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element(By.ID, 'result')
    print(result.get_attribute('innerHTML'))
    assert '44' == result.get_attribute('innerHTML')
    result2 = driver.find_element(By.ID, 'result2').get_attribute('innerHTML')
    print(result2)
    assert 'function mytest() { [native code] }' == result2
    result3 = driver.find_element(By.ID, 'result3').get_attribute('innerHTML')
    result4 = driver.find_element(By.ID, 'result4').get_attribute('innerHTML')
    assert '44' == result3
    assert 'function testinner() { [native code] }' == result4
finally:
    driver.quit()