import time
import os
import subprocess
import platform
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
from selenium.webdriver.common.by import By
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_experimental_option('excludeSwitches', ['ignore-certificate-errors'])
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
port = str(utils.free_port())
server = subprocess.Popen(['python3', 'https-server.py', port])

def read_ca_cert():
    return read_and_replace_file('ca.crt', '\n', '')

def read_and_replace_file(filename, old, new):
    with open(filename, 'r') as fd:
        content = fd.read().replace(old, new)
    return content

def write_file(filename, content):
    with open(filename, 'w') as fd:
        fd.write(content)
index_html = read_and_replace_file('index.tpl', '{port}', port)
write_file('index.html', index_html)

# test with trust anchors
cert = read_ca_cert()

pkg_with_trust_anchors = '''
{
  "name": "test_additional_trust_anchors",
  "main": "index.html",
  "additional_trust_anchors": ["%s"]
}
''' % cert

write_file('package.json', pkg_with_trust_anchors)
try:
    driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
    driver.implicitly_wait(5)
    try:
        print(driver.current_url)
        timeout = 5
        ret = ''
        elem_id = 'result'
        while timeout > 0:
            try:
                driver.switch_to.frame(driver.find_element(By.TAG_NAME, 'iframe'))
                ret = driver.find_element(By.ID, elem_id).get_attribute('innerHTML')
                break
            except selenium.common.exceptions.NoSuchElementException:
                pass
            time.sleep(1)
            timeout = timeout - 1
            if timeout <= 0:
                raise Exception('Timeout when waiting for element' + elem_id)
        title = driver.execute_script('return document.title')
        print(title)
        print(ret)
        assert 'success' in ret
        driver.close()
    finally:
        driver.quit()
    pkg_without_trust_anchors = '\n{\n    "name": "test_additional_trust_anchors",\n    "main": "index.html"\n}\n'
    write_file('package.json', pkg_without_trust_anchors)

    driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log', port=0)
    driver.implicitly_wait(5)
    try:
        print(driver.current_url)
        driver.switch_to.frame(driver.find_element(By.ID, 'test-frame'))
        title = driver.execute_script('return document.title')
        print(title)
        assert not 'test.html' in title
        driver.close()
    finally:
        driver.quit()
finally:
    print('killing child process', server.pid)
    server.kill()
