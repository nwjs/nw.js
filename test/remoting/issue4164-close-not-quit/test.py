import time
import os
import platform
import subprocess
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

def list_process(driver):
    if platform.system() == 'Windows':
        pgrep = subprocess.Popen(['wmic', 'process', 'where', '(ParentProcessId=%s)' % driver.service.process.pid, 'get', 'ProcessId'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out, err = pgrep.communicate()
        print 'live chrome processes:\n%s' % out
        # expect "No Instance(s) Available." in output
        assert('No Instance(s) Available.' in out)
    else:
        pgrep = subprocess.Popen(['pgrep', '-P', str(driver.service.process.pid)], stdout=subprocess.PIPE)
        out, err = pgrep.communicate()
        print 'live chrome processes:\n%s' % out
        print 'pgrep exit with %s' % pgrep.returncode
        # expect exit 1 from pgrep, which means no chrome process alive
        assert(pgrep.returncode == 1)

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(10)
time.sleep(1)
try:
    print driver.current_url
    for handle in reversed(driver.window_handles):
        driver.switch_to_window(handle)
        driver.close()
        
    time.sleep(3) # wait for quit
    list_process(driver)
    
finally:
    driver.quit()
