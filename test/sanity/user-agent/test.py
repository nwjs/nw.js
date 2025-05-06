import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
server = subprocess.Popen(['python3', '-u', 'echo-user-agent.py'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
print(server.stdout.readline())
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