import time
import os
import subprocess
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

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
var http = nw.require('http');
var fs = nw.require('fs');

function testgc() {
  gc();
  document.write('<h1 id="res">success</h1>');
}

var file = fs.createWriteStream("out.png");
var req = http.get("http://127.0.0.1:%s/g.png", function (res) {
    res.pipe(file);
    setTimeout(testgc, 1000);
});
</script>
''' % (port))
    
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
try:
    print driver.current_url
    result = wait_for_element_id(driver, 'res')
    print result
    assert("success" in result)
finally:
    server.terminate()
    driver.quit()

