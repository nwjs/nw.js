import traceback
import time
import os
from selenium import webdriver

#path = os.getcwd();
#path = os.path.join(path, 'tmp-nw', 'chromedriver2_server');
path = '/home/owen-cloud/Desktop/kevin/node-webkit/tests/tmp-nw/chromedriver2_server';

try:
    driver = webdriver.Chrome(path);
    driver.get('http://www.google.com');
    assert driver.title == 'Google'

    time.sleep(5) # Let the user actually see something!
    search_box = driver.find_element_by_name('q')
    search_box.send_keys('ChromeDriver')
    search_box.submit()
    time.sleep(5) # Let the user actually see something!    
    assert driver.title[0:12] == 'ChromeDriver'
    driver.quit()
except:
    driver.quit();
    traceback.print_exc()
else:
    print 'pass'
