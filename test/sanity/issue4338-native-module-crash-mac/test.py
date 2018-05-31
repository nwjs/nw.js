import os
import sys
import shutil
import platform

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

if platform.system() != "Darwin":
    print "Skipped for not Mac platform"
    sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
test_dir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + test_dir)
module_dir = os.path.join(test_dir, "node_module")

try:
    shutil.rmtree(module_dir)
except:
    pass

install_native_modules()

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options)
driver.implicitly_wait(2)

try:
    wait_window_handles(driver, 2)
    handles = driver.window_handles
    print handles
    print driver.current_url
    print "switch to devtools"
    switch_to_devtools(driver)
    print "click console panel"
    devtools_click_tab(driver, "console")
    print "check if 'COMMAND' + 'q' works fine"
    elems = driver.find_elements_by_class_name("console-message-text")
    output = ""
    for i in range(len(elems)):
        if "Board" in elems[i].get_attribute("innerHTML"):
            output = elems[i].get_attribute("innerHTML")
    #print output
    assert("Board" in output)
finally:
    driver.quit()
