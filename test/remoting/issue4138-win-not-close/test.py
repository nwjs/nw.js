import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    driver.find_element_by_tag_name('button').click()
    time.sleep(2) # wait for window open
    print driver.window_handles
    print 'switch_to'
    driver.switch_to.window('child')
    print 'try close'
    driver.close()
    time.sleep(2) # wait for window close
    assert (len(driver.window_handles) == 1)
finally:
    driver.quit()
