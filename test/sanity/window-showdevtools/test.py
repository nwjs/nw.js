import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(
    service=Service(os.environ['CHROMEDRIVER']),
    options=chrome_options
)
driver.implicitly_wait(5)
try:
    flavor = driver.find_element(By.ID, 'flavor').get_attribute('innerHTML')
    assert(flavor == 'normal')
finally:
    driver.quit()
