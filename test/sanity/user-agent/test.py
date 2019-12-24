import time
import os
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

server = subprocess.Popen(['python', '-u', 'echo-user-agent.py'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
print server.stdout.readline()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    print driver.current_url
    user_agent = server.stdout.readline()
    print "user agent: " + user_agent
    server.terminate()
    assert("test-agent" in user_agent)
finally:
    driver.quit()
