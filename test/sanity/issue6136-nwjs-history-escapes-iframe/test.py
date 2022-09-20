import time
import os
import platform
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)

    print('Click Next button')
    driver.find_element_by_tag_name('a').click()
    output = wait_for_element_tag(driver, 'h1')
    assert('Second Page' in output)
    assert(driver.find_element_by_tag_name('iframe') is not None)

    print('Switch to iframe')
    driver.switch_to.frame(driver.find_element_by_tag_name('iframe'))
    result1 = wait_for_element_tag(driver, 'h1')
    assert('Iframe index' in result1)

    print('Click Next button in Iframe index page')
    driver.find_element_by_tag_name('a').click()

    print('Switch to second iframe')
    result2 = wait_for_element_tag(driver, 'h1')
    assert('Iframe Second Page' in result2)

    print('waiting for back to iframe page')
    timeout = 4
    while timeout > 0:
        try:
            output = driver.find_element_by_tag_name('h1').get_attribute('innerHTML')
            assert('Iframe Second Page' in output)
            break
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for element')
    print('Return to inframe page')

finally:
    driver.quit()
