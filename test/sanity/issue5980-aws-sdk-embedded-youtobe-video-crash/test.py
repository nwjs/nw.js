import time
import os
import platform
import sys

if platform.system() != 'Linux':
    print 'Skipped for non Linux platform'
    sys.exit(0)

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
node_module = os.path.join(testdir, "node_modules")
os.chdir(testdir)

install_native_modules()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print driver.current_url
    print 'waiting for crash'
    wait_for_element_tag(driver, "iframe")
    assert(driver.find_element_by_tag_name("iframe") is not None)
    driver.switch_to_frame(driver.find_element_by_tag_name("iframe"))
    wait_for_element_class(driver, "ytp-large-play-button")
    assert(driver.find_element_by_class_name("ytp-large-play-button") is not None)
    print 'There is no crash'
finally:
    driver.quit()
