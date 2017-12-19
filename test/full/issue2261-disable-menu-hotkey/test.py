import time
import os
import pyautogui

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
time.sleep(1)
try:
    driver.implicitly_wait(10)
    print driver.current_url
    # try enable
    driver.find_element_by_id('enable-item').click()
    pyautogui.hotkey('ctrl', 'alt', 'f', interval=0.1)
    result = driver.find_element_by_id('result_1').get_attribute('innerHTML')
    print result
    assert('success' in result)
    # try disable
    driver.find_element_by_id('disable-item').click()
    pyautogui.hotkey('ctrl', 'alt', 'f', interval=0.1)
    elements = driver.find_elements_by_id('result_2')
    assert(len(elements) == 0)
    # try enable again
    driver.find_element_by_id('enable-item').click()
    pyautogui.hotkey('ctrl', 'alt', 'f', interval=0.1)
    result = driver.find_element_by_id('result_2').get_attribute('innerHTML')
    print result
    assert('success' in result)
finally:
    driver.quit()
