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

def test_shortcut_create(keys, expect="success"):
    key = '+'.join(keys)
    id = 'create-' + '-'.join(keys)
    reg_script = 'createShortcut("%s", "%s")' % (id, key)
    print reg_script
    driver.execute_script(reg_script)
    assert_dom(id, expect)


driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    time.sleep(1)
    driver.implicitly_wait(10)

    test_shortcut_create(['a'])
    test_shortcut_create(['keyb'])
    test_shortcut_create(['Numpad1'])
    test_shortcut_create(['Escape'])
    test_shortcut_create(['`'])
    test_shortcut_create(['mediatracknext'])
    test_shortcut_create(['MediaNextTrack'])
    test_shortcut_create(['f1'])
    test_shortcut_create(['f24'])
    test_shortcut_create(['up'])
    test_shortcut_create(['arrowleft'])
    test_shortcut_create(['comma'])
    test_shortcut_create(['.'])

    test_shortcut_create([], 'failure')
    test_shortcut_create(['ctrl'], 'failure')
    test_shortcut_create(['ctrl+alt'], 'failure')
    test_shortcut_create(['command+alt'], 'failure')
    
finally:
    driver.quit()
