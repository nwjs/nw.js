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
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
capabilities = {'pageLoadStrategy': 'none'}
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port = str(utils.free_port())
server = subprocess.Popen(['python3', 'http-server.py', port])
html = open('index.html', 'w')
html.write('\n<script>\nvar http = nw.require(\'http\');\nvar fs = nw.require(\'fs\');\n\nfunction testgc() {\n  gc();\n  document.write(\'<h1 id="res">success</h1>\');\n}\n\nvar file = fs.createWriteStream("out.png");\nvar req = http.get("http://127.0.0.1:%s/g.png", function (res) {\n    res.pipe(file);\n    setTimeout(testgc, 1000);\n});\n</script>\n' % port)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log', additional_capabilities=capabilities)
try:
    print(driver.current_url)
    result = wait_for_element_id(driver, 'res')
    print(result)
    assert 'success' in result
finally:
    server.terminate()
    driver.quit()