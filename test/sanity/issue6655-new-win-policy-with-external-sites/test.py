import os
import sys
import subprocess

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.common import utils
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

os.chdir(testdir)

port = str(utils.free_port())
server_f = os.path.join(testdir, "../", "http-server.py")
server = subprocess.Popen(['python', server_f, port])

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url
    wait_window_handles(driver, 3)
    handles = driver.window_handles
    print handles
    print "switch to test.html page"
    wait_switch_window_name(driver, "test0")
    wait_for_element_id(driver, "blank")
    print "click link with target='_blank'"
    driver.find_element_by_id("blank").click()
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check if event 'new-win-policy' whether fired"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    if len(elems) > 1:
        for i in range(len(elems)):
            if "externalWindow" in elems[i].get_attribute("innerHTML"):
                output = elems[i].get_attribute("innerHTML")
                break
    else:
        output = elems[0].get_attribute("innerHTML")
    print output
    assert("externalWindow" in output)
    assert(len(driver.window_handles) is 3)

finally:
    server.terminate()
    driver.quit()
