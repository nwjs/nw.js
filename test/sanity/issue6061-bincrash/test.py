import time
import os
import shutil
import subprocess
import platform
import sys
from subprocess import Popen, PIPE
from selenium.webdriver.common.by import By

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
binfile = os.path.join(testdir, "index.bin")
nwjc = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), "nwjc.exe" if os.name == "nt" else "nwjc")
os.chdir(testdir)
try:
  os.remove(binfile)
except:
  pass
assert(False == os.path.isfile(binfile))
subprocess.call([nwjc, "index.js", "index.bin"])
assert(os.path.isfile(binfile))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
try:
    switch_to_app(driver)
    print(driver.current_url)
    print('wait for devtools open')
    wait_window_handles(driver, 2)
    print(driver.window_handles)
    print('switch to devtools')
    switch_to_devtools(driver)
    print('click Console panel')
    devtools_click_tab(driver, 'console')
    wait_for_execute_script(driver, 'document.querySelector(".console-object").click()')
    time.sleep(2)
    wait_for_execute_script(driver, 'document.querySelector("#console-messages > div.console-group.console-group-messages > div > div > div > div > div > div:nth-child(1) > span > span.console-message-text > div").shadowRoot.querySelector("div > ol > ol > li > span").click()')

    time.sleep(2)
    wait_for_execute_script(driver, 'document.querySelector("#console-messages > div.console-group.console-group-messages > div > div > div > div > div > div:nth-child(1) > span > span.console-message-text > div").shadowRoot.querySelector("div > ol > ol > ol > li:nth-child(3) > span").click()')
    time.sleep(2)
finally:
    driver.quit()
