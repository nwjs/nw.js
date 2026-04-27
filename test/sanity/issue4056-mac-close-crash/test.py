import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import platform
import sys
if platform.system() != 'Darwin':
    print('Skipped for non Mac platform')
    sys.exit(0)
import selenium.common.exceptions
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    driver.find_element(By.ID, 'winclose').click()
    time.sleep(1)
    try:
        result = driver.find_element(By.ID, 'result')
        assert 'success' in result.get_attribute('innerHTML')
    except selenium.common.exceptions.NoSuchWindowException:
        pass
finally:
    driver.quit()