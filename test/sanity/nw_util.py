import time
import platform
import subprocess

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

def switch_to_devtools(driver, devtools_window=None):
    if devtools_window:
        driver.switch_to_window(devtools_window)

    # necessary compatible for older alphaN
    # where devtools is loaded in an iframe
    inspector_frames = driver.find_elements_by_id('inspector-app-iframe')
    if inspector_frames:
        driver.switch_to_frame(inspector_frames[0])

    # wait for devtools is completely loaded
    while driver.execute_script('return document.readyState') != 'complete':
        time.sleep(1)

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
