import time
import os
import sys
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    wait_for_element_id_content(driver, 'result', 'DOM onload event<br>NW.js loaded event<br>')
    driver.find_element_by_tag_name('a').click()
    time.sleep(1)
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print(result)
    assert result == 'DOM onload event<br>NW.js loaded event<br>'
finally:
    driver.quit()