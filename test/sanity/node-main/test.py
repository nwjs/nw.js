import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

port = str(utils.free_port())

tpl = open('index_html.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.html', 'w')
html.write(content)
html.close()

tpl = open('index_js.tpl', 'r')
content = tpl.read().replace('{port}', port)
tpl.close()

html = open('index.js', 'w')
html.write(content)
html.close()

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    driver.implicitly_wait(10)
    driver.find_element_by_tag_name('button').click()
    result = 'node-main test'
    counter = 0
    while not 'success' in result and counter < 10:
        time.sleep(1)
        result = driver.find_element_by_id('result').get_attribute('innerHTML')
        print result
        counter = counter + 1
    assert('success' in result)
finally:
    driver.quit()
