import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert("success" in result)
    old_handle = driver.current_window_handle
    driver.find_element_by_id('reloadapp').click()
    print 'wait for app reload'
    while len(driver.window_handles) == 0 or driver.window_handles[0] == old_handle:
        time.sleep(1)
    # devtools will be opened as the first window handle
    print driver.window_handles
    print 'wait for devtools window ready'
    driver.switch_to_window(driver.window_handles[0])
    while driver.execute_script('return document.readyState') != 'complete':
        time.sleep(1)
    print 'close devtools'
    driver.close()
    print driver.window_handles
    print 'switch to main window'
    driver.switch_to_window(driver.window_handles[0])
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert("success" in result)    
finally:
    driver.quit()
