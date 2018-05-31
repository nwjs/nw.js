import time
import os
import sys
import subprocess

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

port = str(utils.free_port())
server_file = os.path.join(testdir, "../", "http-server.py")
server = subprocess.Popen(["python", server_file, port])

tpl = open("index.tpl", "r")
content = tpl.read().replace("{port}", port)
tpl.close()

html = open("index.html", "w")
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)

try:
    switch_to_app(driver)
    print driver.current_url
    print "wait for devtools open"
    wait_window_handles(driver, 2)
    print driver.window_handles
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check if event 'connect' whether fired"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = []
    timeout = 10
    while timeout > 0:
        for i in range(len(elems)):
            elem = elems[i].get_attribute("innerHTML")
            output.append(elem)

        if "START" in str(output) and "STOP" in str(output) and "content" in str(output):
            print "all of webview events are fired"
            print str(output)
            break
        else:
            print str(output)
            print "waiting for all of event fired"
            time.sleep(1)
            timeout = timeout - 1
            elems = driver.find_elements_by_class_name("console-message-text")
            if timeout <= 0:
                raise Exception('Timeout when waiting for events')

    assert("START" in str(output))
    assert("STOP" in str(output))
    assert("content" in str(output))
finally:
    server.terminate()
    driver.quit()
