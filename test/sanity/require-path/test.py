import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(5)
try:
    app_path = os.path.dirname(os.path.abspath(__file__))
    public_path = os.path.join(app_path, 'public')
    print('current url: %s' % driver.current_url)
    print('public path: %s' % public_path)
    btn = driver.find_element_by_id('btn')
    btn.click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print('require path: %s' % result)
    assert public_path == result
    driver.find_element_by_id('lnk').click()
    print('current url: %s' % driver.current_url)
    btn = driver.find_element_by_id('btn')
    btn.click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print('require path: %s' % result)
    assert public_path == result
finally:
    driver.quit()