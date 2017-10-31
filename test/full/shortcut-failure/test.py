import time
import os
import pyautogui

from selenium import webdriver
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

def assert_dom(id, expect):
    elem = driver.find_element_by_id(id)
    print elem.get_attribute('innerHTML')
    assert(expect in elem.get_attribute('innerHTML'))

def test_reg(keys, pykeys=None, expect="success"):
    key = '+'.join(keys)
    id = 'reg-' + '-'.join(keys)
    reg_script = 'reg("%s", "%s")' % (id, key)
    print reg_script
    driver.execute_script(reg_script)
    if pykeys is not None:
        pyautogui.hotkey(*pykeys)
    if expect is not None:
        assert_dom(id, expect)

def test_unreg(keys, expect="success"):
    key = '+'.join(keys)
    id = 'unreg-' + '-'.join(keys)
    unreg_script = 'unreg("%s", "%s")' % (id, key)
    print unreg_script
    driver.execute_script(unreg_script)
    assert_dom(id, expect)

def test_reg_twice(keys1, keys2):
	test_reg(keys1, expect=None)
	test_reg(keys2, expect="failure")
	test_unreg(keys1)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    time.sleep(1)
    driver.implicitly_wait(10)

    test_reg_twice(['a'], ['a'])
    test_reg_twice(['a'], ['keya'])
    test_reg_twice(['keya'], ['a'])
    test_reg_twice(['1'], ['digit1'])
    test_reg_twice(['comma'], [','])
    test_reg_twice(['up'], ['arrowup'])
    test_reg_twice(['mediatracknext'], ['medianexttrack'])
    test_reg_twice(['mediaprevtrack'], ['mediatrackprevious'])
    test_reg_twice(['ctrl', 'shift', 'b'], ['shift', 'ctrl', 'b'])

    test_unreg(['ctrl','shift','p'], expect="failure")
finally:
    driver.quit()
