import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(3)
try:
    print(driver.current_url)
    ret1 = ''
    ret2 = ''
    timeout = 10
    while timeout > 0:
        try:
            ret2 = driver.find_element_by_id('request2').get_attribute('innerHTML')
            ret1 = driver.find_element_by_id('request1').get_attribute('innerHTML')
            if (ret2 != ''):
                print('show webview delayed value')
                print(ret2)
                print('show request normatl value')
                print(ret1)
                break
        except selenium.common.exceptions.NoSuchElementException:
            pass

        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for element' + elem_id)

    assert('onRequest' in ret2)
    assert('onRequest' in ret1)

finally:
    driver.quit()
