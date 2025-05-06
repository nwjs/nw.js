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
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
binfile = os.path.join(testdir, 'lib.bin')
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwjc.exe' if os.name == 'nt' else 'nwjc')
os.chdir(testdir)
try:
    os.remove(binfile)
except:
    pass
assert False == os.path.isfile(binfile)
subprocess.call([nwjc, '--nw-module', 'lib2.js', 'lib.bin'])
assert os.path.isfile(binfile)
if platform.system() == 'Linux':
    proc = Popen(['strings', 'lib.bin'], stdout=PIPE, stderr=PIPE)
    out, err = proc.communicate()
    ret = out.decode()
    print(ret)
    assert '42' not in ret
    assert 'foo' not in ret
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'result', '52')
    print(result)
    result2 = wait_for_element_id_content(driver, 'source', 'native code')
    print(result2)
    assert '{ [native code] }' in result2
finally:
    driver.quit()