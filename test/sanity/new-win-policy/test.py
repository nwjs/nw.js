import time
import os
import re
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

def check_size(text, w, h):
    # window sizes are in DIPs; with a fractional device scale factor
    # the physical size can't be represented exactly and innerWidth/Height
    # may differ from the requested size by a pixel or two, so compare with
    # a small tolerance instead of exact string matching.
    m = re.search(r'size:\s*(\d+),\s*(\d+)', text)
    assert m, 'no size found in: ' + text
    rw, rh = int(m.group(1)), int(m.group(2))
    assert abs(rw - w) <= 2 and abs(rh - h) <= 2, 'expected ~%dx%d, got %dx%d' % (w, h, rw, rh)

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
    check_size(output2, 432, 345)
    driver.close()
    driver.switch_to.window(driver.window_handles[0])
    link = driver.find_element(By.LINK_TEXT, 'new window with size')
    print(link.text)
    link.click()
    wait_window_handles(driver, 2)
    driver.switch_to.window(driver.window_handles[-1])
    output = driver.find_element(By.ID, 'output').get_attribute('innerHTML')
    check_size(output, 321, 123)
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