import time
import os
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.path.abspath(__file__))
# nwdist = os.path.dirname(os.environ['CHROMEDRIVER'])
nwdist = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwdist')

script_list = ['path', 'buffer', 'arraybuffer', 'ajax']

for script in script_list:
  script_path = os.path.join(testdir, script)
  child = subprocess.Popen([os.path.join(nwdist, 'nwjc'), script_path + '.js', script_path + '.bin'])
  child.wait()

chrome_options = Options()
chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
time.sleep(1)
try:
    print driver.current_url
    for script in script_list:
      driver.find_element_by_id('eval-%s' % script).click()
      result = driver.find_element_by_id('%s-result' % script).get_attribute('innerHTML')
      print result
      assert("success" in result)
finally:
    driver.quit()
