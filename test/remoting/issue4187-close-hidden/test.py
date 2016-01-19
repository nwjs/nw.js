import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
time.sleep(1)
try:
    print driver.current_url
    time.sleep(1) # wait for window open
    print 'Wait for splash window close'
    while len(driver.window_handles) != 1:
        time.sleep(1)
    print driver.window_handles
    driver.switch_to_window(driver.window_handles[0])
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
finally:
    driver.quit()
