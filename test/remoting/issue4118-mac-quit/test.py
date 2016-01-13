import time
import os
import pyautogui
import platform
import subprocess
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

if platform.system() != "Darwin":
    sys.exit(0)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(10)
time.sleep(1)
try:
    print driver.current_url
    # pyautogui.hotkey('command', 'q')
    print 'key sent'
    time.sleep(3) # wait for quit
    pgrep = subprocess.Popen(['pgrep', '-P', str(driver.service.process.pid)], stdout=subprocess.PIPE)
    out, err = pgrep.communicate()
    print 'live chrome processes:\n%s' % out
    print 'pgrep exit with %s' % pgrep.returncode
    # expect exit 1 from pgrep, which means no chrome process alive
    assert(pgrep.returncode == 1)
finally:
    driver.quit()
