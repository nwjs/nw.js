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
    time.sleep(2) # wait for background devtools open
    driver.switch_to_window(driver.window_handles[-1])
    # necessary compatible for older alphaN
    # where devtools is loaded in an iframe
    inspector_frames = driver.find_elements_by_id('inspector-app-iframe')
    if inspector_frames:
        driver.switch_to_frame(inspector_frames[0])
    driver.execute_script('return document.querySelector(".inspector-view-tabbed-pane").shadowRoot.getElementById("tab-console")').click()
    driver.switch_to_active_element().send_keys('chrome\n')
    driver.find_element_by_class_name('console-object-preview').click()
    time.sleep(1) # wait for crash!
    expanded = driver.find_element_by_css_selector('.console-view-object-properties-section.expanded')
    assert(expanded is not None)
finally:
    driver.quit()
