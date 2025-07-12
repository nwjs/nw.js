import time
import platform
import subprocess
import selenium
from selenium.webdriver.common.action_chains import ActionChains
import argparse
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By

import logging
import os
import sys
import shutil
import tempfile
from subprocess import Popen, PIPE

def get_configured_webdriver(
    chrome_options_instance: Options = None,
    driver_path: str = None,
    base_service_args: list = None,
    log_file_path: str = None,
    additional_capabilities: dict = None,
    port: int = None
):
    _port_arg_parser = argparse.ArgumentParser(prog='webdriver_setup_port_parser', add_help=False)
    _port_arg_parser.add_argument(
        "--chromedriver-port",
        type=int,
        default=None,
        help="Port for Chromedriver. Overrides CHROMEDRIVER_PORT environment variable."
    )
    _known_cli_args, _ = _port_arg_parser.parse_known_args()

    if port is not None:
        _target_port = port
    else:
        _target_port = _known_cli_args.chromedriver_port
    if _target_port is None:
        env_port_str = os.environ.get('CHROMEDRIVER_PORT')
        if env_port_str:
            try:
                _target_port = int(env_port_str)
            except ValueError:
                print(f"[WebDriver Setup] Warning: Invalid CHROMEDRIVER_PORT '{env_port_str}'. Letting Selenium pick.")
                _target_port = None

    _final_service_args = list(base_service_args) if base_service_args else []

    if _target_port:
        _final_service_args.append(f"--port={_target_port}")
        print(f"[WebDriver Setup] Using Chromedriver port: {_target_port}")
    else:
        _target_port = 0
        print(f"[WebDriver Setup] No specific port provided. Letting Selenium choose a free port.")

    if driver_path is None:
        chromedriver_exe_path = os.environ.get('CHROMEDRIVER')
        if not chromedriver_exe_path:
            raise ValueError("[WebDriver Setup] Critical: CHROMEDRIVER environment variable is not set.")
    else:
        chromedriver_exe_path = driver_path

    _chrome_service = Service(
        executable_path=chromedriver_exe_path,
        port=_target_port,
        service_args=_final_service_args if _final_service_args else None,
        log_path=log_file_path,
    )

    if additional_capabilities and isinstance(additional_capabilities, dict):
        for key, value in additional_capabilities.items():
            # The modern way to add capabilities that don't have a dedicated method
            chrome_options_instance.set_capability(key, value)
    try:
        if sys.platform == "win32":
            _chrome_service.creationflags = subprocess.CREATE_NO_WINDOW
        driver_instance = webdriver.Chrome(service=_chrome_service, options=chrome_options_instance)
        print(f"[WebDriver Setup] Chromedriver started successfully on port: {driver_instance.service.port}")
    except Exception as e:
        print(f"[WebDriver Setup] Error during WebDriver initialization: {e}")
        if log_file_path and os.path.exists(log_file_path):
            try:
                with open(log_file_path, 'r') as f_log:
                    log_content = f_log.read()[-1000:]
                print(f"[WebDriver Setup] Last 1000 chars from Chromedriver log ({log_file_path}):\n{log_content}")
            except Exception as e_log:
                print(f"[WebDriver Setup] Could not read Chromedriver log: {e_log}")
        raise
        
    return driver_instance

def find_executable(executable, path=None):
    """Find if 'executable' can be run. Looks for it in 'path'
    (string that lists directories separated by 'os.pathsep';
    defaults to os.environ['PATH']). Checks for all executable
    extensions. Returns full path or None if no command is found.
    """
    if path is None:
        path = os.environ['PATH']
    paths = path.split(os.pathsep)
    extlist = ['']
    if os.name == 'os2':
        (base, ext) = os.path.splitext(executable)
        # executable files on OS/2 can have an arbitrary extension, but
        # .exe is automatically appended if no dot is present in the name
        if not ext:
            executable = executable + ".exe"
    elif sys.platform == 'win32':
        pathext = os.environ['PATHEXT'].lower().split(os.pathsep)
        (base, ext) = os.path.splitext(executable)
        if ext.lower() not in pathext:
            extlist = pathext
    for ext in extlist:
        execname = executable + ext
        if os.path.isfile(execname):
            return execname
        else:
            for p in paths:
                f = os.path.join(p, execname)
                if os.path.isfile(f):
                    return f
    else:
        return None

def start_http_server():
    port_n = 0
    try:
        os.remove('port.txt')
    except:
        pass
    server = subprocess.Popen(['python3', '../http-server-node.py'])
    while not os.path.exists('port.txt'):
        time.sleep(1)
    with open('port.txt', 'r') as file:
        port = file.read().strip()
        port_n = int(port)
    return port_n, server

def stop_http_server(server):
    if sys.platform in ('win32', 'cygwin'):
        os.popen('taskkill /T /F /PID %d' % server.pid)
    else:
        server.terminate()

def install_node_modules():
    npm_path = find_executable('npm')
    npm_cmdline = [npm_path, 'install']

    print("npm_path: ", npm_path)

    proc = Popen(npm_cmdline, stdout=PIPE, stderr=PIPE, env=os.environ)
    out, err = proc.communicate()
    print(out)
    print(err)
    assert(proc.returncode == 0)

def install_native_modules():
    nw_dir  = os.path.normpath(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
    nw_tools = os.path.join(nw_dir, 'tools')
    sys.path.append(nw_tools)
    import getnwversion
    import getnwisrelease

    header_path = os.path.join(nw_dir, 'tmp', 'node')

    nw_version = getnwversion.nw_version
    if getnwisrelease.release == 0:
        nw_version += getnwisrelease.postfix

    arch = ''
    _arch = platform.architecture()[0]
    if _arch == '64bit':
        arch = 'x64'
    elif _arch == '32bit':
        arch = 'ia32'
    else:
        print('Unsupported arch: ' + _arch)
        exit(-1)

    target_arch = arch
    if 'target_arch=ia32' in os.getenv('GYP_DEFINES'):
        target_arch='ia32'

    nw_gyp_path = find_executable('nw-gyp')
    npm_path = find_executable('npm')
    npm_cmdline = [npm_path, 'install']

    if sys.platform in ('win32', 'cygwin'):
        nw_gyp_path = os.path.join(os.path.dirname(nw_gyp_path),
                                   'node_modules', 'nw-gyp', 'bin', 'nw-gyp.js')
        npm_cmdline = [npm_path, 'install', '--msvs_version=2022']

    print("nw_gyp: ", nw_gyp_path)
    print("npm_path: ", npm_path)
    print("header path: ", header_path)
    print("command line: ", npm_cmdline)

    npm_env = {'npm_config_nodedir': header_path, 'npm_config_target': nw_version,
               'npm_config_arch': arch, 'npm_config_target_arch': target_arch,
               'npm_config_runtime': 'node-webkit', 'npm_config_build_from_source': "true",
               'npm_config_node_gyp': nw_gyp_path, 'PATH': os.getenv('PATH')}

    os.environ.update(npm_env)

    proc = Popen(npm_cmdline, stdout=PIPE, stderr=PIPE, env=os.environ)
    out, err = proc.communicate()
    print(out)
    print(err)
    assert(proc.returncode == 0)

def wait_for_execute_script(driver, script, timeout=10):
    ret = None
    while timeout > 0:
        try:
            ret = driver.execute_script(script)
            break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
             raise Exception('Timeout when execute script: ' + script)
    return ret

def wait_for_element_id(driver, elem_id, timeout=10):
    ret = ''
    while timeout > 0:
        try:
            ret = driver.find_element(By.ID, elem_id).get_attribute('innerHTML')
            break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
             raise Exception('Timeout when waiting for element' + elem_id)
    return ret

def wait_for_element_class(driver, elem_class, timeout=10):
    ret = ''
    while timeout > 0:
        try:
            ret = driver.find_element(By.CLASS_NAME, elem_class).get_attribute('innerHTML')
            break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
             raise Exception('Timeout when waiting for element' + elem_class)
    return ret

def wait_for_element_tag(driver, elem_tag, timeout=10):
    ret = ''
    while timeout > 0:
        try:
            ret = driver.find_element(By.TAG_NAME, elem_tag).get_attribute('innerHTML')
            break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        except selenium.common.exceptions.WebDriverException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
             raise Exception('Timeout when waiting for element' + elem_tag)
    return ret

def wait_for_element_id_content(driver, elem_id, content, timeout=60):
    ret = ''
    while timeout > 0:
        try:
            ret = driver.find_element(By.ID, elem_id).get_attribute('innerHTML')
            if content in ret:
              break
        except selenium.common.exceptions.NoSuchElementException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
             raise Exception('Timeout when waiting for element: ' + elem_id + " content: " + content + "; actual: " + ret)
    return ret

# wait for window handles
def wait_window_handles(driver, until, timeout=60):
    n = 0
    if not hasattr(until, '__call__'):
        cond = lambda handles: len(handles) == until
        n = until
    else:
        cond = until
    while not cond(driver.window_handles):
        print(f"waiting for {n} handles: {driver.window_handles}")
        time.sleep(1)
        timeout = timeout - 1
        if timeout == 0:
            raise Exception('Timeout when waiting for window handles')

def wait_switch_window_name(driver, name, timeout=60):
    while timeout > 0:
        try:
            driver.switch_to.window(name)
            break
        except selenium.common.exceptions.NoSuchWindowException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for window handles')

def wait_switch_window_url(driver, path, timeout=120):
    while timeout > 0:
        try:
            for handle in driver.window_handles:
                driver.switch_to.window(handle)
                if driver.current_url.endswith(path) :
                    return
        except selenium.common.exceptions.NoSuchWindowException:
            pass
        time.sleep(1)
        timeout = timeout - 1
        if timeout <= 0:
            raise Exception('Timeout when waiting for window handles')


def switch_to_app(driver, window_handle=None):
    def is_app_url(url):
        return (url.startswith('chrome-extension://') or url.startswith('file://') or url.startswith('http://') or url.startswith('https://')) and not url.endswith('/_generated_background_page.html')

    if window_handle is not None:
        driver.switch_to.window(window_handle)

    if is_app_url(driver.current_url):
        return

    elif window_handle is not None: # raise exception when given window is a devtools window
        raise Exception('Provided window handle is not an app window. %s' % driver.current_url)

    for handle in driver.window_handles:
        driver.switch_to.window(handle)
        if is_app_url(driver.current_url):
            return

    raise Exception('No app window found.')

def switch_to_devtools(driver, devtools_window=None, skip_exception=False):
    def wait_for_devtools_ready():
        # necessary compatible for older alphaN
        # where devtools is loaded in an iframe
        inspector_frames = driver.find_elements(By.ID, 'inspector-app-iframe')
        if inspector_frames:
            driver.switch_to.frame(inspector_frames[0])

        # wait for devtools is completely loaded
        while driver.execute_script('return document.readyState') != 'complete':
            time.sleep(1)

    if devtools_window is not None:
        driver.switch_to.window(devtools_window)

    if driver.current_url.startswith('devtools://'):
        wait_for_devtools_ready()
        return
    elif devtools_window is not None: # raise exception when given window is not a devtools
        raise Exception('Provided window handle is not a devtools window. %s' % driver.current_url)

    for handle in driver.window_handles:
        try:
            driver.switch_to.window(handle)
            if driver.current_url.startswith('devtools://'):
                wait_for_devtools_ready()
                return
        except selenium.common.exceptions.WebDriverException:
            if skip_exception:
                pass

    raise Exception('No devtools window found.')

def devtools_click_tab(driver, tab_name):
    timeout = 30
    while timeout > 0:
        try:
            driver.execute_script('return document.querySelector(".tabbed-pane").shadowRoot.getElementById("tab-%s")' % tab_name).click()
            break
        except (selenium.common.exceptions.WebDriverException, AttributeError):
            pass
        time.sleep(1)
        timeout = timeout - 1

def devtools_type_in_console(driver, keys):
    console_prompt = driver.find_element(By.ID, 'console-prompt')
    ActionChains(driver).click(console_prompt).perform()
    ActionChains(driver).send_keys(keys).perform()

def no_live_process(driver, print_if_fail=True):
    if platform.system() == 'Windows':
        pgrep = subprocess.Popen(['wmic', 'process', 'where', '(ParentProcessId=%s)' % driver.service.process.pid, 'get', 'Name,ProcessId'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out, err = pgrep.communicate()
        ret = (b'No Instance(s) Available.' in out)
        if not ret:
            if not (b'nw.exe' in out):
                ret = True
        if not ret and print_if_fail:
            print('live chrome processes:\n%s' % out)
        # expect "No Instance(s) Available." in output
        return ret
    else:
        pgrep = subprocess.Popen(['pgrep', '-P', str(driver.service.process.pid)], stdout=subprocess.PIPE)
        out, err = pgrep.communicate()
        ret = (pgrep.returncode == 1)
        if not ret and print_if_fail:
            print('live chrome processes:\n%s' % out)
            print('pgrep exit with %s' % pgrep.returncode)
        # expect exit 1 from pgrep, which means no chrome process alive
        return ret

def wait_net_service(server, port, timeout=None):
    """ Wait for network service to appear 
        @param timeout: in seconds, if None or 0 wait forever
        @return: True of False, if timeout is None may return only True or
                 throw unhandled network exception
    """
    import socket
    import errno

    if timeout:
        from time import time as now
        # time module is needed to calc timeout shared between two exceptions
        end = now() + timeout

    while True:
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            if timeout:
                next_timeout = end - now()
                if next_timeout < 0:
                    return False
                else:
            	    s.settimeout(next_timeout)
            
            s.connect((server, port))
        
        except socket.timeout as err:
            # this exception occurs only if timeout is set
            if timeout:
                return False
      
        except socket.error as err:
            # catch timeout exception from underlying network library
            # this one is different from socket.timeout
            if type(err.args) != tuple or err[0] != errno.ETIMEDOUT and err[0] != errno.ECONNREFUSED:
                raise
        else:
            s.close()
            return True

_LOGGER = logging.getLogger(os.path.basename(__file__))


class ScopedTempDir(object):
  """A class that creates a scoped temporary directory."""

  def __init__(self):
    self.path_ = None

  def __enter__(self):
    """Creates the temporary directory and initializes |path|."""
    self.path_ = tempfile.mkdtemp(prefix='kasko_integration_')
    return self

  def __exit__(self, *args, **kwargs):
    """Destroys the temporary directory."""
    if self.path_ is None:
      return
    shutil.rmtree(self.path_)

  @property
  def path(self):
    return self.path_

  def release(self):
    path = self.path_
    self.path_ = None
    return path


class ScopedStartStop(object):
  """Utility class for calling 'start' and 'stop' within a scope."""

  def __init__(self, service, start=None, stop=None):
    self.service_ = service

    if start is None:
      self.start_ = lambda x: x.start()
    else:
      self.start_ = start

    if stop is None:
      self.stop_ = lambda x: x.stop()
    else:
      self.stop_ = stop

  def __enter__(self):
    self.start_(self.service_)
    return self

  def __exit__(self, *args, **kwargs):
    if self.service_:
      self.stop_(self.service_)

  @property
  def service(self):
    """Returns the encapsulated service, retaining ownership."""
    return self.service_

  def release(self):
    """Relinquishes ownership of the encapsulated service and returns it."""
    service = self.service_
    self.service_ = None
    return service
