import time
import os
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))
chrome_options.add_experimental_option("excludeSwitches", ["ignore-certificate-errors"]);

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())
server = subprocess.Popen(['python', 'https-server.py', port])

def read_ca_cert():
    return read_and_replace_file('ca.cert.pem', '\n', '')

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
ca = read_ca_cert()
pkg_with_trust_anchors = '''
{
  "name": "test_additional_trust_anchors",
  "main": "index.html",
  "additional_trust_anchors": ["%s"]
}
''' % ca
write_file('package.json', pkg_with_trust_anchors)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    driver.switch_to_frame(driver.find_element_by_id('test-frame'))
    title = driver.execute_script('return document.title')
    print title
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
    driver.close()
finally:
    driver.quit()

# test without trust anchors
pkg_without_trust_anchors = '''
{
  "name": "test_additional_trust_anchors",
  "main": "index.html"
}
'''
write_file('package.json', pkg_without_trust_anchors)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    driver.switch_to_frame(driver.find_element_by_id('test-frame'))
    title = driver.execute_script('return document.title')
    print title
    assert('not available' in title)
    driver.close()
finally:
    server.terminate()
    driver.quit()
