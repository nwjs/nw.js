import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    driver.switch_to_window(driver.window_handles[-1])
    zoom2 = 'zoom level'
    while zoom2 == 'zoom level':
        zoom2 = driver.find_element_by_id('result').get_attribute('innerHTML')
        time.sleep(1)
    print "zoom2: ", zoom2
    driver.switch_to_window(driver.window_handles[0])
    driver.find_element_by_id('get-zoom').click()
    zoom1 = driver.find_element_by_id('result').get_attribute('innerHTML')
    print "zoom1: ", zoom1
    assert(zoom1 == '1')
    assert(zoom2 != '1')
finally:
    driver.quit()
