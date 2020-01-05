import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
try:
    print driver.current_url
    result = driver.find_element_by_id('result-argv').get_attribute('innerHTML')
    print 'nw.App.argv = %s' % result
    assert('nwapp=' not in result)
    result = driver.find_element_by_id('result-fullargv').get_attribute('innerHTML')
    print 'nw.App.fullArgv = %s' % result
    assert('nwapp=' in result)
finally:
    driver.quit()
