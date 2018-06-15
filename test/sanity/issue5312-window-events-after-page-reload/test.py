import time
import os
import sys
import subprocess

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common import utils

test_dir = os.path.dirname(os.path.abspath(__file__))
port = str(utils.free_port())
server_p = os.path.join(test_dir, "../", "http-server.py")
server = subprocess.Popen(["python", server_p, port])

tpl = open("index.tpl", "r")
content = tpl.read().replace("{port}", port)
tpl.close()

html = open("index.html", "w")
html.write(content)
html.close()

chrome_options = Options()
chrome_options.add_argument("nwapp=" + test_dir)

def click_button(elem_id1, elem_id2, content):
    wait_for_element_id_content(driver, elem_id2, content)
    elem = driver.find_element_by_id(elem_id1)
    ActionChains(driver).click(elem).perform()

def check_console_out(text):
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check if event '%s' whether fired" % text
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    if len(elems) > 2:
	    for i in range(len(elems)):
	        if text in elems[i].get_attribute("innerHTML"):
	            output = elems[i].get_attribute("innerHTML")
	            break
    else:
        output = elems[0].get_attribute("innerHTML")
    print output
    return output

def check_event():
    handles = driver.window_handles
    print handles
    print "switch to test window"
    wait_switch_window_name(driver, "test")
    print "click button in test page"
    click_button("btn2", "rsu2", "Test")
    wait_window_handles(driver, 2)
    handles = driver.window_handles
    print "switch to devtools"
    driver.switch_to_window(handles[1])
    ret1 = check_console_out("onloaded")
    assert("onloaded" in ret1)
    ret2 = check_console_out("onclose")
    assert("onclose" in ret2)
    ret3 = check_console_out("onclosed")
    assert("onclosed" in ret3)

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url
    print "wait for devtools open"
    wait_window_handles(driver, 3, 120)
    check_event()
    print "click button in index page to reload this page"
    wait_switch_window_name(driver, "index")
    print driver.current_url
    click_button("btn1", "rsu1", "Index")
    check_event()
finally:
    driver.quit()
