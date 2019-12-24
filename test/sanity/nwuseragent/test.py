import time
import os
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', '-u', 'echo-user-agent.py', port], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
print server.stdout.readline()

html = open('index.html', 'w')
html.write('''
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf8">
    <title>test</title>
  </head>
  <body>
    <iframe id="iframe"  nwuseragent="test-agent" src="http://localhost:%s/iframe.html"></iframe>
  </body>
</html>
''' % (port))

html.close()

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
