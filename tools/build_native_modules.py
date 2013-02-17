import os, re, sys
import subprocess

native_modules = ['nw_test_loop_without_handle',
                  'bignum',
                  ];

script_dir = os.path.dirname(__file__)
native_root = os.path.join(script_dir, os.pardir, 'tests', 'node_modules')
native_root = os.path.normpath(native_root)
native_root = os.path.abspath(native_root)

nw_gyp_script = os.path.normpath(
    os.path.join(script_dir, 
                 os.pardir, 
                 'tests',
                 'node_modules',
                 'nw-gyp',
                 'bin',
                 'nw-gyp.js'))

nw_gyp_script = os.path.abspath(nw_gyp_script)  
cur_dir = os.getcwd()


#get node-webkit target version
nw_readme_md = os.path.join(os.path.dirname(__file__), '..', 'README.md')

f = open(nw_readme_md)

for line in f:
  if re.match('\[v\d*.\d*.\d*[\s\S]*\]', line):
    target = line.split()[0][2:]
    break


exec_args = ['node', nw_gyp_script, 'configure', '--target=%s'%(target), 'build']

win = sys.platform in ('win32', 'cygwin')

if win:
  exec_args.insert(4, '--arch=ia32')


for dir in native_modules:

  if dir == 'bignum' and win:
    continue
  
  native_dir = os.path.join(native_root, dir)
  os.chdir(native_dir)
  
  subprocess.call(exec_args) 
  #os.execl(node_gyp_script, '', 'build')
  

os.chdir(cur_dir)