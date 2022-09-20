import time
import os
import sys
from selenium.webdriver.common.by import By

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
try:
    os.remove('output.pdf')
except:
    pass

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print(driver.current_url)
    driver.find_element(By.XPATH, '//button[text()="test"]').click()
    wait_for_element_id_content(driver, 'ret', 'output.pdf', 20)
    time.sleep(2)
    os.remove('output.pdf')
    time.sleep(0.1)
    result = driver.find_element_by_id('ret').get_attribute('innerHTML')
    print(result)
    assert("filename: output.pdf; event: rename" == result)
finally:
    driver.quit()
