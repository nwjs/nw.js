import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import re
import platform
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))

def check_size(text, w, h):
    # window sizes are in DIPs; with a fractional device scale factor
    # the physical size can't be represented exactly and innerWidth/Height
    # may differ from the requested size by a pixel or two, so compare with
    # a small tolerance instead of exact string matching.
    m = re.match(r'(\d+)x(\d+)$', text.strip())
    assert m, 'unexpected size format: ' + text
    rw, rh = int(m.group(1)), int(m.group(2))
    assert abs(rw - w) <= 2 and abs(rh - h) <= 2, 'expected ~%dx%d, got %dx%d' % (w, h, rw, rh)

print('Open first time')
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(10)
try:
    print(driver.current_url)
    size = driver.find_element(By.ID, 'size').get_attribute('innerHTML')
    print('open size %s' % size)
    driver.find_element(By.ID, 'resize-window').click()
    size = driver.find_element(By.ID, 'resize').get_attribute('innerHTML')
    print('resize to %s' % size)
finally:
    driver.quit()
print('Open second time')
driver = get_configured_webdriver(chrome_options_instance=chrome_options, port=0)
driver.implicitly_wait(10)
try:
    print(driver.current_url)
    size = driver.find_element(By.ID, 'size').get_attribute('innerHTML')
    print('open size %s' % size)
    check_size(size, 666, 333)
finally:
    driver.quit()
