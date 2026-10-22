import json
import os
import platform
import shutil
import subprocess
import time

testdir = os.path.dirname(os.path.abspath(__file__))
os.chdir(testdir)

pkg = os.path.dirname(os.environ['CHROMEDRIVER'])

if platform.system() == 'Darwin':
    exe = os.path.join(pkg, 'nwjs.app', 'Contents', 'MacOS', 'nwjs')
elif platform.system() == 'Linux':
    exe = os.path.join(pkg, 'nw')
else:
    exe = os.path.join(pkg, 'nw.exe')

workdir = os.path.join(testdir, 'tmp')

try:
    shutil.rmtree(workdir)
except OSError:
    pass

os.mkdir(workdir)

def copy_app(target):
    os.mkdir(target)
    for filename in ['index.html', 'package.json']:
        shutil.copy2(os.path.join(testdir, filename),
                     os.path.join(target, filename))

def run_restart_scenario(name, method, launch_mode, app_path, cwd=None):
    scenario_dir = os.path.join(workdir, name)
    datadir = os.path.join(scenario_dir, 'userdata')
    result = os.path.join(scenario_dir, name + '-result.json')
    switch_arg = '--restart-test-flag=' + name
    plain_arg = 'plain-restart-arg-' + name

    os.mkdir(scenario_dir)

    command = [
        exe,
        '--nwjs-test-mode2',
        '--user-data-dir=' + datadir,
        switch_arg,
    ]
    if launch_mode == 'nwapp-switch':
        command.append('--nwapp=' + app_path)
    else:
        command.append(app_path)
    command.append(plain_arg)

    env = dict(os.environ,
               APP_RESTART_TEST_DIR=scenario_dir,
               APP_RESTART_SCENARIO=name,
               APP_RESTART_METHOD=method,
               APP_RESTART_EXPECT_APP_PATH=os.path.abspath(
                   os.path.join(cwd, app_path) if cwd else app_path),
               APP_RESTART_EXPECT_APP_PATH_IN_ARGV=(
                   '1' if launch_mode == 'nwapp-switch' else '0'),
               APP_RESTART_EXPECT_SWITCH=switch_arg,
               APP_RESTART_EXPECT_ARG=plain_arg)

    print('running %s: %s' % (name, command))
    ret = subprocess.call(command, cwd=cwd, env=env)
    assert ret == 0

    deadline = time.time() + 30
    while not os.path.exists(result) and time.time() < deadline:
        time.sleep(1)

    assert os.path.exists(result), '%s did not write a result' % name

    with open(result, 'r') as result_file:
        data = json.load(result_file)

    print(data)
    assert data['success'], '%s failed: %s' % (name, data['message'])

relative_root = os.path.join(workdir, 'relative root')
os.mkdir(relative_root)
relative_app = 'relative app'
copy_app(os.path.join(relative_root, relative_app))
run_restart_scenario('relative-positional', 'nw', 'positional',
                     relative_app, cwd=relative_root)

switch_app = os.path.join(workdir, 'switch app with spaces')
copy_app(switch_app)
run_restart_scenario('nwapp-switch', 'gui', 'nwapp-switch', switch_app)

absolute_app = os.path.join(workdir, 'absolute-app')
copy_app(absolute_app)
run_restart_scenario('absolute-positional', 'nw', 'positional',
                     absolute_app)

shutil.rmtree(workdir)
