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
time.sleep(1)
try:
    print driver.current_url
    time.sleep(1) # wait for window open
    print 'click button to show jailed devtools'
    driver.find_element_by_id('showdevtools').click()
    print 'wait for devtools open'
    wait_window_handles(driver, 2)
    print 'switch to devtools'
    switch_to_devtools(driver, devtools_window=driver.window_handles[-1])
    print 'click Console panel'
    devtools_click_tab(driver, 'console')
    print 'send_keys "location.pathname<enter>"'
    devtools_type_in_console(driver, 'location.pathname\n')
    pathname = driver.find_element_by_css_selector('.console-user-command-result .console-message-text .object-value-string').get_attribute('textContent')
    print pathname
    assert (pathname == '/child.html')
finally:
    driver.quit()
