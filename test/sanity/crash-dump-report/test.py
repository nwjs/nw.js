import time
import os
import shutil
import zipfile
import platform
import subprocess
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import logging
logging.basicConfig(level=logging.DEBUG)

import crash_server
import nw_util
import report


_LOGGER = logging.getLogger(os.path.basename(__file__))

testdir = os.path.dirname(os.path.abspath(__file__))
nwdist = os.path.join(os.path.dirname(os.environ['CHROMEDRIVER']), 'nwdist')

appdir =  os.path.join(testdir, 'app')
pkg1 = os.path.join(testdir, 'pkg1')

try:
    shutil.rmtree(pkg1)
except:
    pass

def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            if not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
                shutil.copy2(s, d)

# create test directory
os.mkdir(pkg1)

# copy nw to test directory
print "copying %s to %s" % (nwdist, pkg1)
copytree(nwdist, pkg1)
pkgname = 'crash-report-test'

# copy app to test directory
if platform.system() == 'Darwin':
    appdest = os.path.join(pkg1, 'nwjs.app', 'Contents', 'Resources', 'app.nw')
    user_data_dir = os.path.join(os.getenv('HOME'), 'Library', 'Application Support', pkgname)
    exe = os.path.join(pkg1, 'nwjs.app', 'Contents', 'MacOS', 'nwjs')
    check_file = os.path.join(user_data_dir, 'Default', 'Web Data')
elif platform.system() == 'Linux':
    appdest = pkg1
    user_data_dir = os.path.join(os.getenv('HOME'), '.config', pkgname)
    exe = os.path.join(pkg1, 'nw')
    check_file = os.path.join(user_data_dir, 'Default', 'Web Data')
else:
    appdest = pkg1
    user_data_dir = os.path.join(os.getenv('LOCALAPPDATA'), pkgname)
    check_file = os.path.join(user_data_dir, 'User Data', 'Default', 'Web Data')
    exe = os.path.join(pkg1, 'nw.exe')

print "copying %s to %s" % (appdir, appdest)
copytree(appdir, appdest)

print "user data dir: %s" % (user_data_dir)

os.chdir(pkg1)

for arg in ['--test-browser-crash', '--test-renderer-crash']:
    try:
        shutil.rmtree(user_data_dir)
    except:
        pass
    server = crash_server.CrashServer()
    with nw_util.ScopedStartStop(server):
        _LOGGER.info('Started server on port %d', server.port)
        manifest = open(os.path.join(appdest, 'package.json'), 'w')
        manifest.write('''
{
  "name":"%s",
  "version": "0.0.12",
  "crash_report_url": "http://localhost:%d/reportloooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooongurl",
  "main": "index.html"
}
''' % (pkgname, server.port))
        manifest.close()
    
        if os.path.exists(user_data_dir):
            time.sleep(7)
            shutil.rmtree(user_data_dir)
        assert not os.path.exists(user_data_dir), "'%s' should not be existed before testing" % user_data_dir

        p = subprocess.Popen([exe, arg])
        _LOGGER.info('Waiting for crash report')
        try:
            if not server.wait_for_report(120):
                raise Exception('No report received.')
        finally:
            p.terminate()
        r = server.crash(0)
        report.LogCrashKeys(r)
        ptype = r['ptype'][0]
        assert(ptype == 'browser' and arg == '--test-browser-crash' or ptype in ['renderer', 'extension'] and arg == '--test-renderer-crash')
        assert(pkgname == r['prod'][0])
        assert(r['ver'][0] == "0.0.12")
        assert(len(r['upload_file_minidump'][0]) > 100000)

