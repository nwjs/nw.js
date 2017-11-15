import time
import os
import shutil
import subprocess
import platform
from subprocess import Popen, PIPE
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))
chrome_options.add_argument("nwapp=" + testdir)
nw_tools  = os.path.normpath(os.path.join(testdir, os.pardir, os.pardir, os.pardir, 'tools'))
sys.path.append(nw_tools)
import getnwversion
import getnwisrelease

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

os.chdir(testdir)

header_path = os.path.abspath("../../../tmp/node")

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
    print 'Unsupported arch: ' + _arch
    exit(-1)

nw_gyp_path = find_executable('nw-gyp')
npm_path = find_executable('npm')
npm_cmdline = [npm_path, 'install']

if sys.platform in ('win32', 'cygwin'):
    nw_gyp_path = os.path.join(os.path.dirname(nw_gyp_path),
                               'node_modules', 'nw-gyp', 'bin', 'nw-gyp.js')
    npm_cmdline = [npm_path, 'install', '--msvs_version=2015']

print "nw_gyp: ", nw_gyp_path
print "npm_path: ", npm_path
print "header path: ", header_path
print "command line: ", npm_cmdline

npm_env = {'npm_config_nodedir': header_path, 'npm_config_target': nw_version,
           'npm_config_arch': arch, 'npm_config_target_arch': arch,
           'npm_config_runtime': 'node-webkit', 'npm_config_build_from_source': "true",
           'npm_config_node_gyp': nw_gyp_path, 'PATH': os.getenv('PATH')}

os.environ.update(npm_env)

proc = Popen(npm_cmdline, stdout=PIPE, stderr=PIPE, env=os.environ)
out, err = proc.communicate()
print out
print err
assert(proc.returncode == 0)

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
try:
    print driver.current_url
    result = driver.find_element_by_id('result').get_attribute('innerHTML')
    print result
    assert("456" == result)
finally:
    driver.quit()
