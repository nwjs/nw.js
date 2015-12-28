import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    app_path = os.path.dirname(os.path.abspath(__file__))
    public_path = os.path.join(app_path, 'public')
    print 'current url: %s' % driver.current_url
    print 'public path: %s' % public_path
    time.sleep(1)
    btn = driver.find_element_by_id('btn')
    btn.click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print 'require path: %s' % result
    assert(public_path == result)

    driver.find_element_by_tag('a').click()
    print 'current url: %s' % driver.current_url
    time.sleep(1)
    btn = driver.find_element_by_id('btn')
    btn.click()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print 'require path: %s' % result
    assert(public_path == result)
finally:
    driver.quit()
