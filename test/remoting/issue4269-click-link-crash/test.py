import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(2)
time.sleep(1)
try:
    print driver.current_url
    while len(driver.window_handles) != 3:
        time.sleep(1) # wait for popup window and devtools open
    print driver.window_handles
    print 'switch to devtools window'
    driver.switch_to_window(driver.window_handles[1]) # devtools comes to the 2nd
    # necessary compatible for older alphaN
    # where devtools is loaded in an iframe
    inspector_frames = driver.find_elements_by_id('inspector-app-iframe')
    if inspector_frames:
        driver.switch_to_frame(inspector_frames[0])
    driver.execute_script('return document.querySelector(".inspector-view-tabbed-pane").shadowRoot.getElementById("tab-console")').click()
    driver.find_element_by_css_selector('.console-message-text .webkit-html-external-link').click()
    time.sleep(2) # wait for popup window open
    assert(len(driver.window_handles) == 4)
    driver.switch_to_window(driver.window_handles[3])
    print driver.current_url
    assert('https://www.google.com' in driver.current_url)
finally:
    driver.quit()
