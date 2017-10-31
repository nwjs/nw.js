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
    switch_to_app(driver)
    print driver.current_url
    driver.find_element_by_id('require').click()
    print 'wait for devtools open'
    wait_window_handles(driver, 2)
    print 'switch to devtools'
    switch_to_devtools(driver)
    devtools_click_tab(driver, 'console')
    driver.find_element_by_class_name('console-message-url').click()
    sources_panel = driver.find_element_by_css_selector('.panel.sources')
    assert(sources_panel is not None)
finally:
    driver.quit()
