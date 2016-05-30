import time
import os
import shutil

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)

user_data_dir =  os.path.join(testdir, 'userdata')
user_data_tmpl = os.path.join(testdir, 'userdata_v12')

try:
    shutil.rmtree(user_data_dir)
except:
    pass
shutil.copytree(user_data_tmpl, user_data_dir)

chrome_options.add_argument("user-data-dir=" + user_data_dir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    time.sleep(1)
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("success" in result.get_attribute('innerHTML'))
finally:
    driver.quit()
