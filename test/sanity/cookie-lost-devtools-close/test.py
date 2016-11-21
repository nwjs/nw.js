import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

port = str(utils.free_port())

pkgjson = '''
{
  "name": "cookie-lost-devtools-close",
  "main": "index.html",
  "bg-script": "bg.js",
  "port": "%s"
}
''' % port

with open('package.json', 'w') as bg:
  bg.write(pkgjson)

idx = 1
def click_expect(expected):
    global idx
    driver.find_element_by_id('get-cookie').click()
    result = driver.find_element_by_id('result-%s' % idx).get_attribute('innerHTML')
    idx += 1
    print result
    assert(expected in result)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
time.sleep(1)
try:
    print driver.current_url
    click_expect('undefined')
    click_expect('sid=1')
    driver.find_element_by_id('open-devtools').click()
    print 'wait for devtools open'
    wait_window_handles(driver, 2)
    click_expect('sid=1')
    print 'close devtools'
    switch_to_devtools(driver, devtools_window=driver.window_handles[-1])
    driver.close()
    driver.switch_to_window(driver.window_handles[0])
    wait_window_handles(driver, 1)
    print 'devtools closed'
    click_expect('sid=1')
    click_expect('sid=1')
finally:
    #time.sleep(50)
    driver.quit()
