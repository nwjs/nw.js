import time
import os
import sys
import shutil

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

testdir = os.path.dirname(os.environ["CHROMEDRIVER"])
casedir = os.path.dirname(os.path.abspath(__file__))
tardir = os.path.join(casedir, "package.nw")
newdir = os.path.join(testdir, "package.nw")
test1 = os.path.join(testdir, "credits.html")

chrome_options = Options()
chrome_options.add_nw_argument("1.html")

try:
    shutil.rmtree(newdir)
except:
    pass

def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        item = os.path.join(src, item)
        shutil.copy(item, dst)

print "copying %s to %s" % (tardir, newdir)
copytree(tardir, testdir)


def readfile():
    fp = os.path.join(testdir, "issue5242.txt")
    f = open(fp, "r")
    return f.read()

######## test not exist file 1.html 
print "#### test not exist file 1.html ####"

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options)
try:
    print driver.current_url
    res = readfile()
    print res
    assert("1.html" in res)
finally:
    driver.quit()

######## test exist file credits.html 

print '**** test exist file credits.html ****'

chrome_options = Options()
chrome_options.add_nw_argument(test1)

driver2 = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options)
try:
    print driver2.current_url
    res = readfile()
    print res
    assert("credits.html" in res)
finally:
    driver2.quit()
