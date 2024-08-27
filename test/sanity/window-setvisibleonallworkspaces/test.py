import os
import platform
import time

if platform.system() == 'Windows':
    print('Skipped for Windows platform')
    sys.exit(0)

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
try:
    result = driver.find_element_by_id('visible_on_all_workspaces').get_attribute('textContent')
    assert(result == 'true')
finally:
    driver.quit()
