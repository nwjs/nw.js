import time
import os
import sys
import subprocess

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.alert import Alert


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
    print driver.current_url
    print "wait for three windows"
    wait_window_handles(driver, 3)
    handles = driver.window_handles
    print handles
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check output in console.log"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    for i in range(len(elems)):
        if "test" in elems[i].get_attribute("innerHTML"):
            output = elems[i].get_attribute("innerHTML")
            break
    print output
    assert("test inject_js_end" in output)
finally:
    server.terminate()
    driver.quit()
