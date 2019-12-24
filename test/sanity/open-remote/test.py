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

capabilities = {"pageLoadStrategy": "none"}

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', 'http-server.py', port])

html = open('index.html', 'w')
html.write('''
<script>
window.name = 'local';
nw.Window.open('http://localhost:%s/remote.html', function(win) {
  document.write('<h1 id="res">returned window is ' + typeof win + '</h1>');
  win.y = 0;
});
</script>
''' % (port))
    
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities)
try:
    wait_switch_window_name(driver, 'local')
    print driver.current_url
    result = wait_for_element_id(driver, 'res')
    print 'result=' + result
    assert("object" in result)
    wait_switch_window_name(driver, 'remote')
    for id in ['res', 'res2', 'res3']:
        result = wait_for_element_id(driver, id)
        print result
        assert("DISABLED" in result)
finally:
    server.terminate()
    driver.quit()

