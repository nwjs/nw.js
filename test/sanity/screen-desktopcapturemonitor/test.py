import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

def assert_event_fired(event_name):
    tmp_window_handle = None
    try:
        tmp_window_handle = driver.current_window_handle
    except:
        pass
    driver.switch_to_window(base_window)
    assert("success" in driver.find_element_by_id(event_name).get_attribute('innerHTML'))
    print "%s triggered" % event_name
    if tmp_window_handle is not None:
        driver.switch_to_window(tmp_window_handle)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    driver.implicitly_wait(10);
    base_window = driver.current_window_handle
    print "startMonitor"
    driver.find_element_by_id('startMonitor').click()

    print "newWindow"
    driver.find_element_by_id('newWindow').click()
    assert_event_fired('added')
    driver.switch_to_window('mypopup');

    print "trigger orderchanged"
    time.sleep(2) # wait 2s for remembering current window order
    driver.switch_to_window(base_window)
    driver.execute_script('nw.Window.get().focus()')
    assert_event_fired('orderchanged')

    print "trigger namechanged"
    driver.switch_to_window('mypopup')
    driver.find_element_by_id('changeTitle').click()
    assert_event_fired('namechanged')

    print "trigger removed"
    driver.close()
    assert_event_fired('removed')

    print "get result"
    driver.switch_to_window(base_window)
    driver.find_element_by_id('stopMonitor').click()
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("success" in result.get_attribute('innerHTML'))
finally:
    driver.quit()