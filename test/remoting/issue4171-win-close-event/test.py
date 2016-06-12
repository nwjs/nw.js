import time
import os
import platform

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

def click_close():
    system = platform.system()
    if system == 'Linux':
        from linux import WinManager
        WinManager().close_window()
    elif system == 'Windows':
        from win import WinManager
        WinManager().close_window()
    elif system == 'Darwin':
        from mac import WinManager
        WinManager().close_window()
    else:
        print 'Skip for unsupported platform %s' % system
        return
    print 'close window'

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    click_close()
    result = driver.find_element_by_id('result-2').get_attribute('innerHTML')
    print result
    assert('success' in result)
    click_close()
    result = driver.find_element_by_id('result-1').get_attribute('innerHTML')
    print result
    assert('success' in result)
    driver.close()
finally:
    driver.quit()
