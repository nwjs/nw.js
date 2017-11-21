import time
import os
import shutil
import subprocess
import platform
from subprocess import Popen, PIPE

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
binfile = os.path.join(testdir, "nw.bin")
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), "nwjc.exe" if os.name == "nt" else "nwjc")
os.chdir(testdir)
try:
  os.remove(binfile)
except:
  pass
assert(False == os.path.isfile(binfile))
subprocess.call([nwjc, "mytest.js", "nw.bin"])
assert(os.path.isfile(binfile))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    result = driver.find_element_by_id('result')
    print result.get_attribute('innerHTML')
    assert("840" == result.get_attribute('innerHTML'))
finally:
    driver.quit()
