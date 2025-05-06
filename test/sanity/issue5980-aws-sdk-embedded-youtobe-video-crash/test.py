import time
import os
import platform
import sys
if platform.system() != 'Linux':
    print('Skipped for non Linux platform')
    sys.exit(0)
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument('nwapp=' + testdir)
node_module = os.path.join(testdir, 'node_modules')
os.chdir(testdir)
install_native_modules()
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print(driver.current_url)
    print('waiting for crash')
    wait_for_element_id(driver, 'iframe_a')
    driver.switch_to.frame('iframe_a')
    timeout = 10
    while timeout > 0:
        try:
            ret = driver.find_element_by_class_name('ytp-large-play-button')
            break
        except selenium.common.exceptions.NoSuchElementException:
            driver.switch_to_default_content()
            driver.switch_to.frame('iframe_a')
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for element ytp-large-play-button')
    assert driver.find_element_by_class_name('ytp-large-play-button') is not None
    print('There is no crash')
finally:
    driver.quit()