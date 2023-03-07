import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(
    service=Service(
        executable_path=os.environ["CHROMEDRIVER"]
    ),
    options=chrome_options
)
driver.implicitly_wait(5)
try:
    flavor = driver.find_element(By.ID, 'flavor').get_attribute('innerHTML')
    if (flavor == "normal"):
        result = driver.find_element(By.ID, 'result').get_attribute('innerHTML')
        assert(result == 'showDevTools does not exist on normal build flavor.')
finally:
    driver.quit()
