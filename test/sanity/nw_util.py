import time
import platform
import subprocess
import selenium
from selenium.webdriver.common.action_chains import ActionChains

# wait for window handles
def wait_window_handles(driver, until, timeout=60):
    if not hasattr(until, '__call__'):
        cond = lambda handles: len(handles) == until
    else:
        cond = until
    while not cond(driver.window_handles):
        time.sleep(1)
        timeout = timeout - 1
        if timeout == 0:
            raise Exception('Timeout when waiting for window handles')

def wait_switch_window_name(driver, name, timeout=60):
    while timeout > 0:
        try:
            driver.switch_to_window(name)
            break
        except selenium.common.exceptions.NoSuchWindowException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for window handles')


def switch_to_app(driver, window_handle=None):
    def is_app_url(url):
        return url.startswith('chrome-extension://') or url.startswith('file://') or url.startswith('http://') or url.startswith('https://')

    if window_handle is not None:
        driver.switch_to_window(window_handle)

    if is_app_url(driver.current_url):
        return

    elif window_handle is not None: # raise exception when given window is a devtools window
        raise Exception('Provided window handle is not an app window. %s' % driver.current_url)

    for handle in driver.window_handles:
        driver.switch_to_window(handle)
        if is_app_url(driver.current_url):
            return

    raise Exception('No app window found.')

def switch_to_devtools(driver, devtools_window=None):
    def wait_for_devtools_ready():
        # necessary compatible for older alphaN
        # where devtools is loaded in an iframe
        inspector_frames = driver.find_elements_by_id('inspector-app-iframe')
        if inspector_frames:
            driver.switch_to_frame(inspector_frames[0])

        # wait for devtools is completely loaded
        while driver.execute_script('return document.readyState') != 'complete':
            time.sleep(1)

    if devtools_window is not None:
        driver.switch_to_window(devtools_window)

    if driver.current_url.startswith('chrome-devtools://'):
        wait_for_devtools_ready()
        return
    elif devtools_window is not None: # raise exception when given window is not a devtools
        raise Exception('Provided window handle is not a devtools window. %s' % driver.current_url)

    for handle in driver.window_handles:
        driver.switch_to_window(handle)
        if driver.current_url.startswith('chrome-devtools://'):
            wait_for_devtools_ready()
            return

    raise Exception('No devtools window found.')

def devtools_click_tab(driver, tab_name):
    driver.execute_script('return document.querySelector(".tabbed-pane").shadowRoot.getElementById("tab-%s")' % tab_name).click()

def devtools_type_in_console(driver, keys):
    console_prompt = driver.find_element_by_id('console-prompt')
    ActionChains(driver).click(console_prompt).send_keys(keys).perform()

def no_live_process(driver, print_if_fail=True):
    if platform.system() == 'Windows':
        pgrep = subprocess.Popen(['wmic', 'process', 'where', '(ParentProcessId=%s)' % driver.service.process.pid, 'get', 'ProcessId'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out, err = pgrep.communicate()
        ret = ('No Instance(s) Available.' in out)
        if not ret and print_if_fail:
            print 'live chrome processes:\n%s' % out
        # expect "No Instance(s) Available." in output
        return ret
    else:
        pgrep = subprocess.Popen(['pgrep', '-P', str(driver.service.process.pid)], stdout=subprocess.PIPE)
        out, err = pgrep.communicate()
        ret = (pgrep.returncode == 1)
        if not ret and print_if_fail:
            print 'live chrome processes:\n%s' % out
            print 'pgrep exit with %s' % pgrep.returncode
        # expect exit 1 from pgrep, which means no chrome process alive
        return ret
