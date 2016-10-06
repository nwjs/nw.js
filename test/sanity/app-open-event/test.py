import time
import os
import subprocess

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_argument("user-data-dir=" + os.path.join(testdir, 'userdata'))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
time.sleep(1)
try:
    print driver.current_url
    second_instance = subprocess.Popen(['python', 'second_instance.py'])
    second_instance.wait()
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert("success" in result)
    assert("--flag-switches-begin" not in result)
finally:
    driver.quit()
