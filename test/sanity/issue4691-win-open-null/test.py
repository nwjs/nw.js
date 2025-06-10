import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    driver.find_element(By.ID, 'new-inst').click()
    result = driver.find_element(By.ID, 'new-inst-result').get_attribute('innerHTML')
    print('opening new instance returned "%s"' % result)
    assert 'success' in result
    driver.find_element(By.ID, 'same-inst').click()
    result = driver.find_element(By.ID, 'same-inst-result').get_attribute('innerHTML')
    print('opening same instance returned "%s"' % result)
    assert 'success' in result
finally:
    driver.quit()