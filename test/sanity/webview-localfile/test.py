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
try:
    print driver.current_url
    wait_switch_window_name(driver, 'webview1')
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert('success' in result)
    print 'checking titles to ensure 1.html is not loaded in untrusted webview'
    counter = 0
    for handle in driver.window_handles:
        driver.switch_to_window(handle)
        title = driver.title
        print 'title: ' + title
        if 'title of 1.html' in title:
            counter = counter + 1
    assert(counter == 1)
    print 'launch cdt'
    driver.switch_to_window('main')
    driver.find_element_by_tag_name('button').click() #launch cdt
    counter = 15
    while counter > 0:
        try:
            print 'switch to cdt'
            switch_to_devtools(driver, None, True)
            break
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        counter = counter - 1

    print 'click Elements panel'
    devtools_click_tab(driver, 'elements')
    print 'find h1'
    h1 = driver.execute_script('return document.getElementById("elements-content").firstChild.shadowRoot.querySelectorAll(".webkit-html-text-node")[1]').get_attribute('textContent')
    print h1
    assert (h1 == 'success')
finally:
    driver.quit()
