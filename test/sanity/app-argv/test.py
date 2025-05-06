import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('result-argv').get_attribute('innerHTML')
    print('nw.App.argv = %s' % result)
    assert 'nwapp=' not in result
    result = driver.find_element_by_id('result-fullargv').get_attribute('innerHTML')
    print('nw.App.fullArgv = %s' % result)
    assert 'nwapp=' in result
finally:
    driver.quit()