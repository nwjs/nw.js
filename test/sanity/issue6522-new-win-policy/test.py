import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys 
from selenium.webdriver.common.alert import Alert

chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)

def click(element_id, num):
    element_id = driver.find_element_by_id(element_id)
    if num > 1:
        ActionChains(driver).key_down(Keys.CONTROL).click(element_id).perform()
    else:
        ActionChains(driver).click(element_id).perform()
    out = Alert(driver).text
    print out
    assert("link open action intercepted!" in out)
    print "Accept alert 'OK'"
    Alert(driver).accept()

driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
driver.implicitly_wait(2)
try:
    print driver.current_url
    print "ctrl + click on third tag <a>"
    click("third", 3)

    print "Normal click on first tag <a>"
    click("first", 1)

    print "ctrl + click on second tag <a>"
    click("second", 2)
finally:
    driver.quit()
