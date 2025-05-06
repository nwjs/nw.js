import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
test_dir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument('nwapp=' + test_dir)
port = str(utils.free_port())
pkgjson = '\n{\n  "name": "cookie-lost-devtools-close",\n  "main": "index.html",\n  "bg-script": "bg.js",\n  "port": "%s"\n}\n' % port
with open(os.path.join(test_dir, 'package.json'), 'w') as bg:
    bg.write(pkgjson)
idx = 1

def click_expect(expected):
    global idx
    driver.find_element_by_id('get-cookie').click()
    result = wait_for_element_id(driver, 'result-%s' % idx)
    idx += 1
    print(result)
    assert expected in result
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print(driver.current_url)
    click_expect('undefined')
    click_expect('sid=1')
    driver.find_element_by_id('open-devtools').click()
    print('wait for devtools open')
    wait_window_handles(driver, 2)
    click_expect('sid=1')
    print('close devtools')
    switch_to_devtools(driver)
    driver.close()
    driver.switch_to.window(driver.window_handles[0])
    wait_window_handles(driver, 1)
    print('devtools closed')
    click_expect('sid=1')
    click_expect('sid=1')
finally:
    driver.quit()