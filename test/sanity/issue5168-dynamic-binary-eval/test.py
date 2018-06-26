import os
import subprocess
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
foobin = os.path.join(testdir, "foo.bin")
barbin = os.path.join(testdir, "bar.bin")
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']),
       "nwjc.exe" if os.name == "nt" else "nwjc")
os.chdir(testdir)

try:
  os.remove(foobin)
  os.remove(barbin)
except:
  pass

assert(False == os.path.isfile(foobin))
assert(False == os.path.isfile(barbin))
subprocess.call([nwjc, "foo.js", "foo.bin"])
subprocess.call([nwjc, "bar.js", "bar.bin"])
assert(os.path.isfile(foobin))
assert(os.path.isfile(barbin))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    print driver.current_url
    switch_to_app(driver)
    wait_for_element_id_content(driver, "btn", "Test")
    driver.find_element_by_id("btn").click()
    print "wait for devtools open"
    wait_window_handles(driver, 2)
    handles = driver.window_handles
    print handles
    wait_switch_window_name(driver, handles[0])
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check output message in console panel"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = []
    if len(elems) > 5:
        for i in range(len(elems)):
            output.append(elems[i].get_attribute("innerHTML"))
    else:
        output = elems[0].get_attribute("innerHTML")
    print output
    assert("foo.bin" in output[0])
    assert("foo" in output[1])
    assert("bar.bin" in output[2])
    assert("bar" in output[3])
    assert("foo.bin" in output[4])
    assert("foo" in output[5])
    assert("bar.bin" in output[6])
    assert("bar" in output[7])

finally:
    driver.quit()