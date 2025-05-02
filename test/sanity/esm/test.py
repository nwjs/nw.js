import time
import os
import sys
import re
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

test_dir = os.path.dirname(os.path.abspath(__file__))
chrome_options = Options()
chrome_options.add_argument("nwapp=" + test_dir)

pkgjson = '''
{
  "name": "test-esm",
  "main": "main.html",
  "chromium-args": "--enable-features=NWESM",
  "node-main": "index.js"
}
'''

with open(os.path.join(test_dir, 'package.json'), 'w') as bg:
  bg.write(pkgjson)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'output', 'test object=')
    print(result)
    assert("typeof test=object" in result)
    assert("Hello world" in result)
    assert("Failed to resolve module specifier 'fs'" in result)
    assert(re.search(r'"meta-url":"file://.*?/src/content/nw/test/sanity/esm/test\.mjs"', result))

finally:
    driver.quit()

pkgjson = '''
{
  "name": "test-esm",
  "main": "main.html",
  "chromium-args": "--enable-features=NWESM,NWChainImportNode",
  "node-main": "index.js"
}
'''

with open(os.path.join(test_dir, 'package.json'), 'w') as bg:
  bg.write(pkgjson)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'output', 'test object=')
    print(result)
    assert("typeof test=object" in result)
    assert("Hello world" in result)
    assert("enable-features=NWESM,NWChainImportNode" in result)
    assert(re.search(r'"meta-url":"file://.*?/src/content/nw/test/sanity/esm/test\.mjs"', result))

finally:
    driver.quit()
