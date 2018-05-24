import os
import sys
import shutil
import platform

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

pkgname = 'issue4637'

if platform.system() == "Windows":
    user_data_dir = os.path.join(os.getenv('LOCALAPPDATA'), pkgname, "User Data")
elif platform.system() == 'Linux':
    user_data_dir = os.path.join(os.getenv('HOME'), '.config', pkgname)
else:
    user_data_dir = os.path.join(os.getenv('HOME'), 'Library', 'Application Support', pkgname)

log_file = os.path.join(user_data_dir, "chrome_debug.log")

try:
    shutil.rmtree(user_data_dir)
except:
    pass

def check_cache():
    if os.path.exists(log_file):
        print "Debug log file is enabled"
        assert(False)
    else:
        print "Debug log file is disabled"
        assert(True)


chrome_options.add_argument("user-data-dir=" + user_data_dir)

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)

try:
    print driver.current_url
    check_cache()
    output = wait_for_element_id_content(driver, "result", "Success")
    assert("Success" in output)
finally:
    driver.quit()
