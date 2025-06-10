import os
import sys
import shutil
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

datadir = os.path.join(testdir, 'userdata')
try:
    shutil.rmtree(datadir)
except:
    pass

try:
    os.remove('last_pos.txt')
except:
    pass
try:
    os.remove('succeed.txt')
except:
    pass
try:
    os.remove('failed.txt')
except:
    pass

pkg1 = os.path.dirname(os.environ['CHROMEDRIVER'])

if platform.system() == 'Darwin':
    exe = os.path.join(pkg1, 'nwjs.app', 'Contents', 'MacOS', 'nwjs')
elif platform.system() == 'Linux':
    exe = os.path.join(pkg1, 'nw')
else:
    exe = os.path.join(pkg1, 'nw.exe')


p = subprocess.call([exe, "--user-data-dir="+datadir, "."])

assert(os.path.exists('last_pos.txt'))

p = subprocess.call([exe, "--user-data-dir="+datadir, "."])

if (os.path.exists('failed.txt')) :
    failed = open('failed.txt', 'r')
    print("failed: " + failed.read())
    failed.close()

assert(os.path.exists('succeed.txt'))
