import time
import os
import platform
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print((driver.current_url))
    print('wait for devtools open')
    wait_window_handles(driver,2)
    handles = driver.window_handles
    print(handles)
    driver.switch_to.window(handles[0])
    print('click button to execute code')
    driver.find_element_by_tag_name('button').click()
    print('get all keys in window')
    result = driver.find_element_by_class_name('log').get_attribute('innerText')
    assert('postMessage' in result)
finally:
    driver.quit()
