import time
import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    print("waiting for crash")
    time.sleep(3)
    res = wait_for_element_id(driver, "result")
    assert("success" in res)
    print("There is no crash")
finally:
    driver.quit()