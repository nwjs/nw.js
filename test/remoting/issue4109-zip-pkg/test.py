import time
import os
import tempfile

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from zipfile import ZipFile

package_file = os.path.join(tempfile.mkdtemp(), 'package.nw')
with ZipFile(package_file, 'w') as myzip:
  myzip.write('package.json')
  myzip.write('index.html')
script_dir = os.path.normcase(os.path.normpath(os.path.dirname(os.path.abspath(__file__))))
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.join(script_dir, package_file))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    print 'package file: %s' % package_file
    print 'script dir: %s' % script_dir
    result = os.path.normcase(os.path.normpath(driver.find_element_by_id('result').get_attribute('innerHTML')))
    print 'extracted dir: %s' % result
    assert(script_dir not in result)
finally:
    driver.quit()
