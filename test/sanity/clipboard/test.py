import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

def assert_dom(id, expect):
    elem = driver.find_element_by_id(id)
    print elem.get_attribute('innerHTML')
    assert(expect in elem.get_attribute('innerHTML'))

def assert_dom_not(id, not_expect):
    elem = driver.find_element_by_id(id)
    print elem.get_attribute('innerHTML')
    assert(not_expect not in elem.get_attribute('innerHTML'))

def test_get_text():
  id = 'test-get-text'
  script = 'getText("%s")' % id
  print script
  driver.execute_script(script)
  assert_dom(id, 'success')

def test_set_text():
  id = 'test-set-text-1'
  value = 'abc'
  script = 'setText("%s", "%s")' % (id, value)
  print script
  driver.execute_script(script)
  assert_dom(id, 'success')
  id = 'test-set-text-2'
  script = 'getText("%s")' % id
  print script
  driver.execute_script(script)
  assert_dom(id, 'success')
  assert_dom(id, value)

def test_clear():
  id = 'test-clear-1'
  value = 'abc'
  script = 'setText("%s", "%s")' % (id, value)
  print script
  driver.execute_script(script)
  assert_dom(id, 'success')
  id = 'test-clear-2'
  script = 'getText("%s")' % id
  print script
  driver.execute_script(script)
  assert_dom(id, 'success')
  assert_dom(id, value)
  id = 'test-clear-3'
  script = 'clearClipboard("%s")' % id
  print script
  driver.execute_script(script)
  assert_dom(id, 'success')
  id = 'test-clear-4'
  value = 'abc'
  script = 'getText("%s")' % id
  print script
  driver.execute_script(script)
  assert_dom_not(id, value) 

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    time.sleep(1)

    test_get_text()
    test_set_text()
    test_clear()   
finally:
    driver.quit()
