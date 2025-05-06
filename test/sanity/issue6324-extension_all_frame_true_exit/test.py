import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
test_dir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + test_dir)
chrome_options.add_nw_argument('--load-extension=' + os.path.join(test_dir, 'test_ext/'))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element_by_tag_name('h1').get_attribute('innerHTML')
    print(result)
    assert 'Hello World' in result
    res = driver.find_element_by_tag_name('iframe')
    assert res is not None
finally:
    driver.quit()