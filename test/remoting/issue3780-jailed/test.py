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
    time.sleep(1) # wait for window open
    driver.find_element_by_id('showdevtools').click()
    time.sleep(1) # wait for devtools open
    driver.switch_to_window(driver.window_handles[-1])
    # necessary compatible for older alphaN
    # where devtools is loaded in an iframe
    inspector_frames = driver.find_elements_by_id('inspector-app-iframe')
    if inspector_frames:
        print 'switch to inspector-app-iframe'
        driver.switch_to_frame(inspector_frames[0])
    print 'click Console panel'
    driver.execute_script('return document.querySelector(".inspector-view-tabbed-pane").shadowRoot.getElementById("tab-console")').click()
    print 'send_keys "location.pathname<enter>"'
    driver.switch_to_active_element().send_keys('location.pathname\n')
    pathname = driver.find_element_by_css_selector('.console-user-command-result .console-message-text .object-value-string').get_attribute('textContent')
    print pathname
    assert (pathname == '/child.html')
finally:
    driver.quit()
