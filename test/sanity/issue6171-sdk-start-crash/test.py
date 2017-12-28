import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url
    print 'waiting for crash'
    time.sleep(5)
    result = driver.find_element_by_id('versions').get_attribute('innerHTML')
    #print result
    assert('NWjs version' in result)
    print 'There is no crash'
finally:
    driver.quit()
