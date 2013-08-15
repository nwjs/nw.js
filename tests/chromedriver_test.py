import traceback
import sys
import time
from selenium import webdriver

#pass path of chromdriver
path = sys.argv[1]
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
    
