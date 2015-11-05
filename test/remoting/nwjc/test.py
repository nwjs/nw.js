import time
import os
import shutil
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
binfile = os.path.join(testdir, "mytest.bin")
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), "nwjc.exe" if os.name == "nt" else "nwjc")
try:
  os.remove(binfile)
except:
  pass
assert(False == os.path.isfile(binfile))
subprocess.call([nwjc, "mytest.js", "mytest.bin"])
assert(os.path.isfile(binfile))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    time.sleep(1)
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("44" == result.get_attribute('innerHTML'))
finally:
    driver.quit()
