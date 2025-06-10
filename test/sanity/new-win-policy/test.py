import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
try:
    print(driver.current_url)
    link = driver.find_element(By.LINK_TEXT, 'new window')
    print(link.text)
    link.click()
    wait_window_handles(driver, 2)
    print(driver.window_handles)
    driver.switch_to.window(driver.window_handles[-1])
    output = driver.find_element(By.ID, 'output')
    assert output.get_attribute('value') == '["inject-js-start","body-start","inject-js-end","onload-dom"]'
    output2 = driver.find_element(By.ID, 'output2').get_attribute('innerHTML')
    assert 'Success' in output2
    assert '432, 345' in output2
    driver.close()
    driver.switch_to.window(driver.window_handles[0])
    link = driver.find_element(By.LINK_TEXT, 'new window with size')
    print(link.text)
    link.click()
    wait_window_handles(driver, 2)
    driver.switch_to.window(driver.window_handles[-1])
    output = driver.find_element(By.ID, 'output').get_attribute('innerHTML')
    assert '321, 123' in output
    driver.close()
    driver.switch_to.window(driver.window_handles[0])
    link = driver.find_element(By.LINK_TEXT, 'new window')
    if platform.system() == 'Darwin':
        ActionChains(driver).key_down(Keys.SHIFT).click(link).key_up(Keys.SHIFT).perform()
    else:
        ActionChains(driver).key_down(Keys.CONTROL).click(link).key_up(Keys.CONTROL).perform()
    wait_window_handles(driver, 2)
    driver.switch_to.window(driver.window_handles[-1])
    output = driver.find_element(By.ID, 'output')
    assert output.get_attribute('value') == '["inject-js-start","body-start","inject-js-end","onload-dom"]'
    output2 = driver.find_element(By.ID, 'output2').get_attribute('innerHTML')
    assert 'Success' in output2
finally:
    driver.quit()