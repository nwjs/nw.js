import time
import os
import urllib.parse, urllib.request, urllib.parse, urllib.error
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

def path2url(path):
    return urllib.parse.urljoin('file:', urllib.request.pathname2url(path))
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
chrome_options = Options()
chrome_options.add_argument('nwapp=' + testdir)
htmlfile = os.path.join(testdir, 'index.html')
localurl = path2url(htmlfile)
tpl = open('package.json.tpl', 'r')
template = tpl.read().replace('{localurl}', localurl)
tpl.close()

def test_pattern(pattern, expect_nw):
    content = template.replace('{pattern}', pattern)
    html = open('package.json', 'w')
    html.write(content)
    html.close()
    driver = get_configured_webdriver(chrome_options_instance=chrome_options)
    driver.implicitly_wait(5)
    try:
        print('testing pattern %s, expect nw: %s' % (pattern, expect_nw))
        result = wait_for_element_id_content(driver, 'nw_access', expect_nw)
    finally:
        driver.quit()
test_pattern('"<all_urls>"', 'yes')
test_pattern('', 'no')
test_pattern('"file://*"', 'yes')
test_pattern('"file:///dir/not/exists/*"', 'no')