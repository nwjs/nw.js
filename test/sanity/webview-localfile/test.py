import time
import os
import urlparse, urllib
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

def path2url(path):
        return urlparse.urljoin(
                  'file:', urllib.pathname2url(path))

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_experimental_option("windowTypes", ["webview"])

capabilities = {"pageLoadStrategy": "none"}

htmlfile = os.path.join(testdir, '1.html')
localurl = path2url(htmlfile)

tpl = open('index.tpl', 'r')
content = tpl.read().replace('{localurl}', localurl)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, desired_capabilities = capabilities, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    wait_switch_window_name(driver, 'webview1')
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
    driver.switch_to_window('main')
    driver.find_element_by_tag_name('button').click() #launch cdt
    time.sleep(3)
    print 'switching to wv2'
    driver.switch_to_window(driver.window_handles[2]) #switch to wv2
    print 'driver.title: ' + driver.title
    assert('title of 1.html' != driver.title)
    driver.switch_to_window(driver.window_handles[1])
    time.sleep(2)
    print 'click Elements panel'
    devtools_click_tab(driver, 'elements')
    print 'find h1'
    h1 = driver.execute_script('return document.getElementById("elements-content").firstChild.shadowRoot.querySelectorAll(".webkit-html-text-node")[1]').get_attribute('textContent')
    print h1
    assert (h1 == 'success')
finally:
    driver.quit()
