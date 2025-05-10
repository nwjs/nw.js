import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nw_util import get_configured_webdriver
import time
import os
import subprocess
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common import utils
chrome_options = Options()
chrome_options.add_argument('nwapp=' + os.path.dirname(os.path.abspath(__file__)))
testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)
server = subprocess.Popen(['python3', 'http-server.py'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, encoding='utf-8')
print('Waiting for server to start and report port...')
port = 0
try:
    port_line = server.stdout.readline()
    if not port_line:
        stderr_output = server.stderr.read()
        print('Server process exited prematurely.', file=sys.stderr)
        print(f'Server stderr:\n{stderr_output}', file=sys.stderr)
        server.wait()
        print(f'Server exit code: {server.returncode}', file=sys.stderr)
        sys.exit(1)
    port = int(port_line.strip())
    print(f'Server reported it is running on port: {port}')
    print('Caller script can now interact with the server.')
    time.sleep(5)
except ValueError:
    print(f"Error: Could not parse port number from server output: '{port_line.strip()}'", file=sys.stderr)
    stderr_output = server.stderr.read()
    print(f'Server stderr:\n{stderr_output}', file=sys.stderr)
    server.terminate()
    server.wait()
    sys.exit(1)
except Exception as e:
    print(f'An unexpected error occurred: {e}', file=sys.stderr)
    server.terminate()
    server.wait()
    sys.exit(1)
manifest = open('package.json', 'w')
manifest.write('\n{\n  "name":"test-node-remote",\n  "node-remote":"<all_urls>",\n  "main":"http://127.0.0.1:%s/index.html"\n}\n' % port)
manifest.close()
driver = get_configured_webdriver(chrome_options_instance=chrome_options, base_service_args=['--verbose'], log_file_path='log')
driver.implicitly_wait(5)
try:
    print(driver.current_url)
    result = driver.find_element_by_id('result')
    print(result.get_attribute('innerHTML'))
    assert 'success' in result.get_attribute('innerHTML')
finally:
    import platform
    if platform.system() == 'Windows':
        subprocess.call(['taskkill', '/F', '/T', '/PID', str(server.pid)])
    else:
        server.terminate()
    driver.quit()
