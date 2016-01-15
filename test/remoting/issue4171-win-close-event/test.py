import time
import os
import pyautogui
import platform

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

def click_close():
    pos = map(int, driver.execute_script('return pos()').split(','))
    system = platform.system()
    if system == 'Linux':
        # close button on top/left. top offset is buggy (see `win-move`)
        x = pos[0] + 10
        y = pos[1] - 10
    elif system == 'Windows':
        # close button on top/right
        x = pos[0] + pos[2] - 10
        y = pos[1] + 10
    elif system == 'Darwin':
        # close button on top/left
        x = pos[0] + 10
        y = pos[1] + 10
    else:
        print 'Skip for unsupported platform %s' % system
        return
    print 'window pos: (%d, %d)' % (pos[0], pos[1])
    print 'window size: (%d, %d)' % (pos[2], pos[3])
    print 'click close at (%d, %d)' % (x, y)
    pyautogui.click(x, y)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    click_close()
    result = driver.find_element_by_id('result-2').get_attribute('innerHTML')
    print result
    assert('success' in result)
    click_close()
    result = driver.find_element_by_id('result-1').get_attribute('innerHTML')
    print result
    assert('success' in result)
    driver.close()
finally:
    driver.quit()
