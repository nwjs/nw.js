import time
import os
import subprocess

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
var http = nw.require('http');
var fs = nw.require('fs');

function testgc() {
  gc();
  document.write('<h1 id="res">success</h1>');
}

var file = fs.createWriteStream("out.png");
var req = http.get("http://localhost:%s/g.png", function (res) {
    res.pipe(file);
    setTimeout(testgc, 1000);
});
</script>
''' % (port))
    
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
time.sleep(1)
try:
    print driver.current_url
    time.sleep(7)
    result = driver.find_element_by_id('res').get_attribute('innerHTML')
    print result
    assert("success" in result)
finally:
    server.terminate()
    driver.quit()

