import time
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_experimental_option('windowTypes', ['webview'])
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
try:
    os.remove('port.txt')
except:
    pass
server = subprocess.Popen(['python', '../http-server-node.py'])
while not os.path.exists('port.txt'):
    time.sleep(1)
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'remote_access', 'yes')
    print('remote_access: %s' % result)
    elems = driver.find_element_by_tag_name('a')
    elems.click()
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'remote_access', 'yes')
    print('remote_access: %s' % result)
    result = wait_for_element_id_content(driver, 'remote_xhr', 'yes')
    print('remote_access with xhr: %s' % result)
finally:
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
    driver.quit()