import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
from selenium import webdriver
from selenium.webdriver.common.by import By
driver = get_configured_webdriver()
try:
    print(driver.current_url)
    id = driver.find_element(By.ID, 'nwjsver')
    print(id.text)
finally:
    driver.quit()
