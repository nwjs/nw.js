import os
import sys
import time
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    # an image splash without width/height takes the size of the image (240x135)
    wait_window_handles(driver, 2, timeout=20)
    wait_switch_window_url(driver, 'splash.png', timeout=20)
    size = wait_for_execute_script(driver, 'return [window.innerWidth, window.innerHeight]')
    print('splash size: %s' % size)
    assert abs(size[0] - 240) <= 1 and abs(size[1] - 135) <= 1
    # closed after the main window has loaded and min_duration has passed;
    # with "show": false the app shows its window itself
    wait_window_handles(driver, 1, timeout=30)
    driver.switch_to.window(driver.window_handles[0])
    assert driver.current_url.endswith('index.html')
    wait_for_element_id_content(driver, 'main', 'shown', timeout=10)
finally:
    driver.quit()
