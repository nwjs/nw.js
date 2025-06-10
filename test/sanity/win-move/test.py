import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import platform
import sys
if platform.system() == 'Linux':
    print('Skiping win-move because of upstream issue. See nwjs/nw.js#4217.')
    sys.exit(0)
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(10)
try:
    print(driver.current_url)
    driver.find_element(By.ID, 'moveto').click()
    result = driver.find_element(By.ID, 'moveto-result').get_attribute('innerHTML')
    print(result)
    assert 'success' in result
    driver.find_element(By.ID, 'moveby').click()
    result = driver.find_element(By.ID, 'moveby-result').get_attribute('innerHTML')
    print(result)
    assert 'success' in result
finally:
    driver.quit()