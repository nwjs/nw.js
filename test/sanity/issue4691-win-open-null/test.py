import time
import os
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    print driver.current_url
    driver.find_element_by_id('new-inst').click()
    result = driver.find_element_by_id('new-inst-result').get_attribute('innerHTML')
    print 'opening new instance returned "%s"' % result
    assert('success' in result)
    driver.find_element_by_id('same-inst').click()
    result = driver.find_element_by_id('same-inst-result').get_attribute('innerHTML')
    print 'opening same instance returned "%s"' % result
    assert('success' in result)
finally:
    driver.quit()
