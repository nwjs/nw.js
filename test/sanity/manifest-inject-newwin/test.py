import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print(driver.current_url)
    link = driver.find_element_by_link_text('new window')
    print(link.text)
    link.click()
    wait_window_handles(driver, 2)
    print(driver.window_handles)
    driver.switch_to.window(driver.window_handles[-1])
    output = driver.find_element_by_id('output')
    assert(output.get_attribute('value') == '["inject-js-start","body-start","inject-js-end","onload-dom"]')

finally:
    driver.quit()
