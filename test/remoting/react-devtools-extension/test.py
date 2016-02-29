import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_argument("load-extension=" + os.path.join(testdir, 'react-devtools'))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    driver.find_element_by_id('showdevtools').click()
    print 'wait for devtools open'
    wait_window_handles(driver, 2)
    print 'switch to devtools'
    switch_to_devtools(driver, devtools_window=driver.window_handles[-1])
    print 'click react panel'
    driver.execute_script('WebInspector.inspectorView._tabbedPane.selectTab(WebInspector.inspectorView._tabbedPane._tabs[9]._id)')
    time.sleep(5)
    driver.switch_to_frame(driver.find_elements_by_class_name("extension")[0])
    success = False
    for s in driver.find_elements_by_tag_name('span'):
            print s.get_attribute('innerHTML')
            if 'trace react updates' in  s.get_attribute('innerHTML'):
                    success = True
                    break
    assert(success)
finally:
    driver.quit()
