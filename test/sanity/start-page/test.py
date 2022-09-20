import time
import os

from selenium import webdriver

driver = webdriver.Chrome(os.environ['CHROMEDRIVER'])
try:
    print(driver.current_url)
    id = driver.find_element_by_id('nwjsver')
    print(id.text)
finally:
    driver.quit()
