import os
import sys
import shutil
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port = str(utils.free_port())

print(shutil.which('python3'))

server = subprocess.Popen([shutil.which('python3'), '-u', 'echo-user-agent.py', port], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
print(server.stdout.readline())
html = open('index.html', 'w')
html.write('\n<!DOCTYPE html>\n<html lang="en">\n  <head>\n    <meta charset="utf8">\n    <title>test</title>\n  </head>\n  <body>\n    <iframe id="iframe"  nwuseragent="test-agent" src="http://localhost:%s/iframe.html"></iframe>\n  </body>\n</html>\n' % port)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    user_agent = server.stdout.readline().decode()
    print('user agent: ' + user_agent)
    server.terminate()
    assert 'test-agent' in user_agent
finally:
    driver.quit()
