import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(service=Service(executable_path=os.environ['CHROMEDRIVER']), options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
try:
    result = driver.find_element(By.ID, 'result-argv').get_attribute("innerHTML")
    assert('nwapp=' not in result)
    result = driver.find_element(By.ID, 'result-fullargv').get_attribute('innerHTML')
    assert('nwapp=' in result)
finally:
    driver.quit()
