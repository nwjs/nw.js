import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url
    time.sleep(1) # wait for window open
    print 'click button to show jailed devtools'
    driver.find_element_by_id('showdevtools').click()
    print 'wait for devtools open'
    wait_window_handles(driver, 2)
    print 'switch to devtools'
    switch_to_devtools(driver, devtools_window=driver.window_handles[-1])
    print 'click Elements panel'
    devtools_click_tab(driver, 'elements')
    print 'find h1'
    h1 = driver.execute_script('return document.getElementById("elements-content").firstChild.shadowRoot.querySelectorAll(".webkit-html-text-node")[1]').get_attribute('textContent')
    print h1
    assert (h1 == 'child')
finally:
    driver.quit()
