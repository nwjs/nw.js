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
    # the splash window is open next to the (hidden) main window
    wait_window_handles(driver, 2, timeout=20)
    wait_switch_window_url(driver, 'splash.html', timeout=20)
    seen = time.time()
    assert 'splash' in wait_for_element_id(driver, 'splash')
    size = wait_for_element_id(driver, 'size')
    print('splash size: ' + size)
    width, height = [int(v) for v in size.split(',')]
    assert abs(width - 320) <= 1 and abs(height - 180) <= 1
    # it is closed once the main window has loaded and min_duration has passed
    wait_window_handles(driver, 1, timeout=30)
    elapsed = time.time() - seen
    print('splash closed after %.1fs' % elapsed)
    assert elapsed >= 2  # min_duration is 4s; allow for polling delay
    driver.switch_to.window(driver.window_handles[0])
    assert driver.current_url.endswith('index.html')
    assert 'main' in wait_for_element_id(driver, 'main')
    assert driver.execute_script('return document.visibilityState') == 'visible'
finally:
    driver.quit()
