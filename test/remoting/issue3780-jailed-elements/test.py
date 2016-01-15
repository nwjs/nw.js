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
    print 'click Elements panel'
    driver.execute_script('return document.querySelector(".inspector-view-tabbed-pane").shadowRoot.getElementById("tab-elements")').click()
    print 'find h1'
    h1 = driver.execute_script('return document.getElementById("elements-content").firstChild.shadowRoot.querySelectorAll(".webkit-html-text-node")[1]').get_attribute('textContent')
    print h1
    assert (h1 == 'child')
finally:
    driver.quit()
