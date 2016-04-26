import time
import os
import platform
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

if platform.system() != 'Darwin':
    print 'skipped for non Mac system'
    sys.exit(0)

def get_title():
    from Foundation import NSAppleScript

    src = '''
    global frontApp, frontAppName, windowTitle
    tell application "System Events"
        set frontApp to first application process whose frontmost is true
        set frontAppName to name of frontApp
        tell process frontAppName
            tell (1st window whose value of attribute "AXMain" is true)
                set windowTitle to value of attribute "AXTitle"
            end tell
        end tell
    end tell
    return windowTitle
    '''

    s = NSAppleScript.alloc().initWithSource_(src)
    r, e = s.executeAndReturnError_(None)
    if e:
        print e['NSAppleScriptErrorBriefMessage']
        raise Exception(e['NSAppleScriptErrorBriefMessage'])
    return r.stringValue()

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
driver.implicitly_wait(5)
time.sleep(1)
try:
    print driver.current_url
    title = get_title()
    print title
    assert(title == "issue4186-mac-title")
finally:
    driver.quit()
