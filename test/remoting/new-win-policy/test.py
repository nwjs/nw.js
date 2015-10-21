import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    link = driver.find_element_by_link_text('new window')
    print link.text
    link.click()
    #raw_input("Press Enter to continue...")
    time.sleep(2)
    print driver.window_handles
    driver.switch_to_window(driver.window_handles[-1])
    output = driver.find_element_by_id('output')
    assert(output.get_attribute('value') == '["inject-js-start","body-start","inject-js-end","onload-dom"]')
finally:
    #time.sleep(50)
    driver.quit()
