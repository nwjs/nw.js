import time
import os
import subprocess
import platform
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys 

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
binfile = os.path.join(testdir, "test1.bin")
nwjc = os.path.join(os.path.dirname(os.environ["CHROMEDRIVER"]), "nwjc.exe" if os.name == "nt" else "nwjc")
os.chdir(testdir)
try:
  os.remove(binfile)
except:
  pass
assert(False == os.path.isfile(binfile))
subprocess.call([nwjc, "test1.js", "test1.bin"])
assert(os.path.isfile(binfile))

def clickResume():
    if platform.system() != "Darwin":
        ActionChains(driver).key_down(Keys.CONTROL).send_keys("\\").key_up(Keys.CONTROL).perform()
    else:
        ActionChains(driver).key_down(Keys.COMMAND).send_keys("\\").key_up(Keys.COMMAND).perform()

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
    print "type foo(foo2) in Console panel"
    devtools_type_in_console(driver, "foo(foo2)\n")
    print "start to debug test2.js via breakpoint"
    time.sleep(1)
    clickResume()
    print "click Console panel"
    devtools_click_tab(driver, "console")
    print "check output 'foo(foo2)' which in Console panel"
    out1 = driver.find_element_by_class_name("source-code").get_attribute("innerHTML")
    assert("foo" in out1)
    assert("foo2" in out1)
    print "check output 'test1' which in Console panel"
    elems = driver.find_elements_by_class_name("console-message-text")
    print "check output 'test2' which in Console panel"
    assert("test1" in elems[0].get_attribute("innerHTML"))
    assert("test2" in elems[1].get_attribute("innerHTML"))
finally:
    driver.quit()