import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_argument("headless=new")
driver = webdriver.Chrome(
    service=Service(
        executable_path=os.environ['CHROMEDRIVER']
        ),
        options=chrome_options
    )

try:
    nw_status = driver.find_element(By.ID, "7994").text
    is_nw_enabled = nw_status == "nw is enabled"
    assert is_nw_enabled
finally:
    driver.quit()
