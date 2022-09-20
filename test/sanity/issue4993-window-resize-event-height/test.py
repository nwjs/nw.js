import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    element = driver.find_element_by_id('resize-window')
    ActionChains(driver).click(element).perform()
    res = wait_for_element_id(driver, "resize")
    print(res)
    assert("width: 7" in res)
    assert("height: 5" in res)
finally:
    driver.quit()
