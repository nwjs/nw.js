import time
import os
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
capabilities = {'pageLoadStrategy': 'none'}
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port, server = start_http_server()

html = open('index.html', 'w')
html.write('\n<script>\nwindow.name = \'local\';\nnw.Window.open(\'http://localhost:%s/remote.html\', function(win) {\n  document.write(\'<h1 id="res">returned window is \' + typeof win + \'</h1>\');\n  win.y = 0;\n});\n</script>\n' % port)
html.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, additional_capabilities=capabilities)
try:
    wait_switch_window_name(driver, 'local')
    print(driver.current_url)
    result = wait_for_element_id(driver, 'res')
    print('result=' + result)
    assert 'object' in result
    wait_switch_window_name(driver, 'remote')
    for id in ['res', 'res2', 'res3']:
        result = wait_for_element_id(driver, id)
        print(result)
        assert 'DISABLED' in result
finally:
    stop_http_server(server)
    driver.quit()
