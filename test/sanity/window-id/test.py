import os
import sys
import shutil
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

datadir = os.path.join(testdir, 'userdata')
try:
    shutil.rmtree(datadir)
except:
    pass

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_argument("user-data-dir=" + datadir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
last_pos = ""
try:
    res = wait_for_element_id(driver, "yellow")
    print res
    assert(res == "800, 600")
    driver.find_element_by_id('resize').click()
    res = wait_for_element_id(driver, "yellow")
    print res
    assert(res == "345, 345")
    driver.find_element_by_id('move').click()
    driver.find_element_by_id('getxy').click()
    res = wait_for_element_id(driver, "xy")
    print res
    last_pos = res

finally:
    driver.quit()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
try:
    res = wait_for_element_id(driver, "yellow")
    print res
    assert(res == "345, 345")
    driver.find_element_by_id('getxy').click()
    res = wait_for_element_id(driver, "xy")
    print res
    assert (res == last_pos)
finally:
    driver.quit()

