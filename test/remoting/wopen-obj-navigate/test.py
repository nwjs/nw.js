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
    driver.find_element_by_tag_name('a').click()  #navigate
    driver.switch_to_window(driver.window_handles[0])
    driver.find_element_by_tag_name('button').click() #get target window
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
    driver.switch_to_window(driver.window_handles[-1])
    driver.find_element_by_tag_name('button').click() #click close button
    driver.switch_to_window(driver.window_handles[0])
    result = driver.find_element_by_id('result2').get_attribute('innerHTML')
    print result
    assert('success' in result)
finally:
    driver.quit()
