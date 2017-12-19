import time
import os
import platform

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

# open first time
print 'Open first time'
driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(10)
time.sleep(1)
try:
    print driver.current_url
    size = driver.find_element_by_id('size').get_attribute('innerHTML')
    print 'open size %s' % size
    driver.find_element_by_id('resize-window').click()
    size = driver.find_element_by_id('resize').get_attribute('innerHTML')
    print 'resize to %s' % size
finally:
    driver.quit()

# open second time
print 'Open second time'
driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(10)
time.sleep(1)
try:
    print driver.current_url
    size = driver.find_element_by_id('size').get_attribute('innerHTML')
    print 'open size %s' % size
    assert(size == '666x333')
finally:
    driver.quit()