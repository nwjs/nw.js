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
chrome_options.add_argument('nwapp=' + test_dir)
pkgjson = '\n{\n  "name": "test-esm",\n  "main": "main.html",\n  "chromium-args": "--enable-features=NWESM",\n  "node-main": "index.js"\n}\n'
with open(os.path.join(test_dir, 'package.json'), 'w') as bg:
    bg.write(pkgjson)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'output', 'test object=')
    print(result)
    assert 'typeof test=object' in result
    assert 'Hello world' in result
    assert "Failed to resolve module specifier 'fs'" in result
    assert re.search('"meta-url":"file://.*?/src/content/nw/test/sanity/esm/test\\.mjs"', result)
finally:
    driver.quit()
pkgjson = '\n{\n  "name": "test-esm",\n  "main": "main.html",\n  "chromium-args": "--enable-features=NWESM,NWChainImportNode",\n  "node-main": "index.js"\n}\n'
with open(os.path.join(test_dir, 'package.json'), 'w') as bg:
    bg.write(pkgjson)
driver = get_configured_webdriver(chrome_options_instance=chrome_options)
driver.implicitly_wait(2)
try:
    print(driver.current_url)
    result = wait_for_element_id_content(driver, 'output', 'test object=')
    print(result)
    assert 'typeof test=object' in result
    assert 'Hello world' in result
    assert 'enable-features=NWESM,NWChainImportNode' in result
    assert re.search('"meta-url":"file://.*?/src/content/nw/test/sanity/esm/test\\.mjs"', result)
finally:
    driver.quit()
