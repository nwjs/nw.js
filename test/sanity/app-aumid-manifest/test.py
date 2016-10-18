import time
import os
import platform
import subprocess
import sys

if platform.system() != 'Windows':
    print 'Skipped for non Windows platform'
    sys.exit(0)

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import *

from selenium import webdriver
from selenium.webdriver.chrome.options import Options

chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

def test_list(package_json, operations):
    idx = [0]

    def click_expect(elem, expect):
        elem.click()
        i = idx[-1] = idx[-1] + 1
        result = driver.find_element_by_id('result-%s' % i).get_attribute('innerHTML')
        print result
        assert(expect in result)

    def open_and_switch(elem):
        elem.click()
        wait_window_handles(driver, 3)
        switch_to_devtools(driver, driver.window_handles[1])
        driver.close()
        wait_window_handles(driver, 2)
        driver.switch_to_window(driver.window_handles[1])
        idx.append(0)

    def close_restore():
        driver.close()
        driver.switch_to_window(driver.window_handles[0])
        idx.pop()

    def fill_input(elem, value):
        elem.send_keys(value)

    def click(elem):
        elem.click()

    with open('package.json', 'w') as manifest:
        manifest.write(package_json)

    driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options, service_log_path="log", service_args=["--verbose"])
    driver.implicitly_wait(2)
    time.sleep(1)
    try:
        print driver.current_url
        for args in operations:
            op = args.pop(0)
            if op != 'close':
                elem = driver.find_element_by_id(args[0])
            else:
                elem = None
            if op == 'click':
                if len(args) == 1:
                    print 'click %s' % args[0]
                    click(elem)
                elif len(args) == 2:
                    print 'click %s and expect %s' % (args[0], args[1])
                    click_expect(elem, args[1])
                else:
                    raise Exception('len(args) > 2 is not supported')
            elif op == 'open':
                print 'click %s and switch to the window' % args[0]
                open_and_switch(elem)
            elif op == 'fill':
                print 'fill %s with %s' % (args[0], args[1])
                fill_input(elem, args[1])
            elif op == 'close':
                print 'close and restore to previous window'
                close_restore()
            else:
                raise Exception('unsupported opcode: %s' % op)
    finally:
        driver.quit()

test_list('''
{
    "name": "app-aumid-manifest",
    "main": "index.html"
}
''', [
    ['click', 'show', '_crx_'],
    ['click', 'show-def', '_crx_'],
    ['open', 'open'],
    ['click', 'show', '_crx_'],
    ['click', 'show-def', '_crx_'],
    ['close'],
])

test_list('''
{
    "name": "app-aumid-manifest-default",
    "main": "index.html",
    "app_user_model_id": "foo"
}
''', [
    ['click', 'show', 'foo'],
    ['click', 'show-def', 'foo'],
    ['open', 'open'],
    ['click', 'show', 'foo'],
    ['click', 'show-def', 'foo'],
    ['close'],
])

test_list('''
{
    "name": "app-aumid-manifest-win",
    "main": "index.html",
    "window": {
        "app_user_model_id": "foo"
    }
}
''', [
    ['click', 'show', 'foo'],
    ['click', 'show-def', '_crx_'],
    ['open', 'open'],
    ['click', 'show', '_crx_'],
    ['click', 'show-def', '_crx_'],
    ['close'],
])

test_list('''
{
    "name": "app-aumid-manifest-default-win",
    "main": "index.html",
    "app_user_model_id": "foo",
    "window": {
        "app_user_model_id": "bar"
    }
}
''', [
    ['click', 'show', 'bar'],
    ['click', 'show-def', 'foo'],
    ['open', 'open'],
    ['click', 'show', 'foo'],
    ['click', 'show-def', 'foo'],
    ['close'],
])

test_list('''
{
    "name": "app-aumid-manifest-set-win",
    "main": "index.html",
    "app_user_model_id": "foo",
    "window": {
        "app_user_model_id": "bar"
    }
}
''', [
    ['click', 'show', 'bar'],
    ['click', 'show-def', 'foo'],
    ['fill', 'aumid', 'baz'],
    ['click', 'set-aumid'],
    ['click', 'show', 'baz'],
    ['click', 'show-def', 'foo'],
    ['open', 'open'],
    ['click', 'show', 'foo'],
    ['click', 'show-def', 'foo'],
    ['fill', 'aumid', 'baz'],
    ['click', 'set-aumid'],
    ['click', 'show', 'baz'],
    ['click', 'show-def', 'foo'],
    ['close'],
])

test_list('''
{
    "name": "app-aumid-manifest-set-default",
    "main": "index.html",
    "app_user_model_id": "foo",
    "window": {
        "app_user_model_id": "bar"
    }
}
''', [
    ['click', 'show', 'bar'],
    ['click', 'show-def', 'foo'],
    ['fill', 'aumid-def', 'far'],
    ['click', 'set-def'],
    ['click', 'show', 'bar'],
    ['click', 'show-def', 'far'],
    ['open', 'open'],
    ['click', 'show', 'far'],
    ['click', 'show-def', 'far'],
    ['fill', 'aumid-def', 'foo'],
    ['click', 'set-def'],
    ['click', 'show', 'far'],
    ['click', 'show-def', 'foo'],
    ['close'],
])

test_list('''
{
    "name": "app-aumid-manifest-open",
    "main": "index.html",
    "app_user_model_id": "foo",
    "window": {
        "app_user_model_id": "bar"
    }
}
''', [
    ['click', 'show', 'bar'],
    ['click', 'show-def', 'foo'],
    ['fill', 'aumid-win', 'baz'],
    ['open', 'open-aumid'],
    ['click', 'show', 'baz'],
    ['click', 'show-def', 'foo'],
    ['close'],
])