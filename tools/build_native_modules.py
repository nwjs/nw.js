#!/usr/bin/env python
import os, re, sys
import subprocess

native_modules = ['nw_test_loop_without_handle',
                  'bignum',
                  'dtrace-provider',
                  'ref',
                  'lame',
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

import optparse
parser = optparse.OptionParser()
parser.add_option('-t','--target',
                  help='the node-webkit verison')
opts, args = parser.parse_args()
if opts.target:
  target = opts.target

exec_args = ['nw-gyp',
             'configure',
             '--target=%s'%(target),
             'build']

win = sys.platform in ('win32', 'cygwin')



#We need to rebuild a submodule in http-auth
apache_crypt_path = os.path.join(
  script_dir,
  "..",
  "tests",
  "node_modules",
  "http-auth",
  "node_modules",
  "htpasswd",
  "node_modules",
  "apache-crypt")
os.chdir(apache_crypt_path)
subprocess.call(exec_args)


for dir in native_modules:

  if dir == 'bignum' and win:
    continue

  native_dir = os.path.join(native_root, dir)
  os.chdir(native_dir)
  #exec_args[1] = os.path.relpath(nw_gyp_script, os.getcwd())
  subprocess.call(exec_args)
  #os.execl(node_gyp_script, '', 'build')



os.chdir(cur_dir)
