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

def test_reg_unreg(keys, pykeys):
    test_reg(keys, pykeys)
    test_unreg(keys)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    time.sleep(1)
    driver.implicitly_wait(10)
    test_reg_unreg(['a'], ['a'])
    test_reg_unreg(['keyb'], ['b'])
    test_reg_unreg(['KeyC'], ['c'])
    test_reg_unreg(['ctrl', 'b'], ['ctrl', 'b'])
    test_reg_unreg(['ctrl','shift','b'], ['ctrl','shift','b'])
    test_reg_unreg(['ctrl','shift','alt','b'], ['ctrl','shift','alt','b'])
    test_reg_unreg(['ctrl','shift','alt','`'], ['ctrl','shift','alt','`'])
    test_reg_unreg(['ctrl','shift','alt','escape'], ['ctrl','shift','alt','escape'])
finally:
    driver.quit()
