import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument('nwapp=' + testdir)
chrome_options.add_argument('load-extension=' + os.path.join(testdir, 'react-devtools'))
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    driver.find_element_by_id('showdevtools').click()
    print('wait for devtools open')
    wait_window_handles(driver, 2)
    print('switch to devtools')
    switch_to_devtools(driver, devtools_window=driver.window_handles[-1])
    print('click react panel')
    driver.execute_script('UI.inspectorView.tabbedPane.selectTab(UI.inspectorView.tabbedPane.tabs[9].id)')
    time.sleep(5)
finally:
    driver.quit()