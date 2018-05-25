import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)

try:
    print driver.current_url
    output = wait_for_element_id_content(driver, "result", "Width: 800, Height: 600")
    print output
    assert("Width: 800, Height: 600" in output)
finally:
    driver.quit()
