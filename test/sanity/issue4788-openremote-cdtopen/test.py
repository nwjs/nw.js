import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.common.exceptions import NoSuchElementException

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

capabilities = {"pageLoadStrategy": "none"}

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', 'http-server.py', port])

html = open('index.html', 'w')
html.write('''
<html>
<body>
<script>
chrome.developerPrivate.openDevTools({renderViewId: -1, renderProcessId: -1, extensionId: chrome.runtime.id});
window.name = "main";
setTimeout(function() { 
  nw.Window.open('http://localhost:%s/remote.html', function(win) {
    document.write('<h1 id="res">returned window is ' + typeof win + '</h1>');
  });
}, 2000);
</script>
</body>
</html>
''' % (port))
    
html.close()

run_counter = 6

try:
    while run_counter > 0 :
        run_counter = run_counter - 1
        driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities)
        time.sleep(1)
        try:
            driver.switch_to_window("main")
            print driver.current_url
            result = ''
            wait_counter = 10
            while wait_counter > 0 :
                try:
                    wait_counter = wait_counter - 1
                    time.sleep(1)
                    result = driver.find_element_by_id('res').get_attribute('innerHTML')
                    break
                except NoSuchElementException:
                    pass
            print result
            assert("object" in result)
        finally:
            driver.quit()

finally:
    server.terminate()
