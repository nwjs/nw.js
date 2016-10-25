import time
import os
import sys
import platform

if platform.system() != 'Linux':
    print 'Skipped for non Linux platform'
    sys.exit(0)

from linux import WinManager;
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options.add_argument("nwapp=" + testdir)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    wm = WinManager()
    win = wm.get_active_window()
    wm_name = wm.get_property('WM_NAME', win=win)
    print 'WM_NAME: %s' % wm_name
    assert('issue4693-xprop' in wm_name)
    wm_class = wm.get_property('WM_CLASS', win=win).split('\0')
    print 'WM_CLASS: %s %s' % (wm_class[0], wm_class[1])
    assert('issue4693-xprop' in wm_class[1])
finally:
    driver.quit()

