import os
import subprocess
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from nw_util import *

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)

port = str(utils.free_port())
server = subprocess.Popen(["python3", "http-server.py", port])

manifest = open('package.json', 'w')
manifest.write('''
{
  "name":"dom-filelist-append",
  "node-remote":[ "<all_urls>", "file://*" ],
  "main":"http://localhost:%s/index.html"
}
''' % (port))
manifest.close()

driver = webdriver.Chrome(
    executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"]
)
driver.implicitly_wait(10)
time.sleep(1)
try:
    driver.get("http://localhost:%s/index.html" % port)
    result = wait_for_element_id_content(driver, "file-input")
    assert isinstance(result, dict)
except:
    assert False
finally:
    server.terminate()
    driver.quit()
