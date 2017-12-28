import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print driver.current_url
    print('Click frame button')
    driver.find_element_by_id('refresh').click()
    output = driver.find_element_by_id('frameInfo').get_attribute('innerHTML')
    assert('Iframe top accessible: true' in output)
    print('Switch to iframe')
    driver.switch_to_frame(driver.find_element_by_tag_name('iframe'))
    print('Click inframe button')
    driver.find_element_by_id('refresh').click()
    result = driver.find_element_by_id('iframeInfo').get_attribute('innerHTML')
    assert('Top accessible: true' in result)
finally:
    driver.quit()
