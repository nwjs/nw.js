#!/usr/bin/env python
import os
import tarfile
import sys
import getnwversion
import shutil
import distutils.core
import re

def update_uvh(tmp_dir, header_files):
  for file in header_files:
    header_f = os.path.join(tmp_dir, 'node', 'src', file)
    rfile = open(header_f, 'r')
    old = rfile.read()
    new = re.sub('third_party/node/deps/uv/include/uv.h', 'uv.h', old, 0)
    wfile = open(header_f, 'w')
    wfile.write(new)
    wfile.close()
  return

script_dir = os.path.dirname(__file__)
nw_root  = os.path.normpath(os.path.join(script_dir, os.pardir))
project_root = os.path.normpath(os.path.join(nw_root, os.pardir, os.pardir)) 
third_party_dir = os.path.normpath(os.path.join(project_root, 'third_party'))
tmp_dir = os.path.normpath(os.path.join(nw_root, 'tmp'))

nw_version = getnwversion.nw_version

#parse command line arguments
'''
-t, the version of nw-headers to download
'''
if '-t' in sys.argv:
  nw_version = sys.argv[sys.argv.index('-t') + 1]
tarname = 'nw-headers-v' + nw_version + '.tar.gz'
tarpath = os.path.join(tmp_dir, tarname)

#make tmpdir
if os.path.exists(tmp_dir):
  pass
else:
  os.mkdir(tmp_dir)

# prepare the files to compress
print 'Begin copy file'
base = os.path.join(third_party_dir, 'node')
for dirpath, dirnames, filenames in os.walk(base):
  relpath = dirpath.replace(third_party_dir + os.sep, '')
  for dirs in dirnames:
    if dirs =='gyp' or dirs == 'gyp_addon':
      try:
        shutil.copytree(os.path.join(dirpath, dirs), os.path.join(tmp_dir, relpath, dirs))
      except:
        distutils.dir_util.copy_tree(os.path.join(dirpath, dirs), os.path.join(tmp_dir, relpath, dirs))
  for files in filenames:
    if files.endswith('.h') or files.endswith('.gypi') or files == 'gyp' or files == 'gyp_addon':
      if not os.path.exists(os.path.join(tmp_dir, relpath)):
        os.makedirs(os.path.join(tmp_dir, relpath))
      shutil.copyfile(os.path.join(dirpath, files), os.path.join(tmp_dir, relpath, files))
shutil.rmtree(os.path.join(tmp_dir, 'node', 'deps', 'v8'))
base = os.path.join(project_root, 'v8')
for dirpath, dirnames, filenames in os.walk(base):
  relpath = dirpath.replace(project_root + os.sep, '')
  relpath = os.path.join('node', 'deps', relpath)
  for dirs in dirnames:
    if dirs == 'gyp' or dirs == 'gyp_addon':
      try:
        shutil.copytree(os.path.join(dirpath, dirs), os.path.join(tmp_dir, relpath, dirs))
      except:
        distutils.dir_util.copy_tree(os.path.join(dirpath, dirs), os.path.join(tmp_dir, relpath, dirs))
  for files in filenames:
    if files.endswith('.h') or files.endswith('.gypi') or files == 'gyp' or files == 'gyp_addon':
      if not os.path.exists(os.path.join(tmp_dir, relpath)):
        os.makedirs(os.path.join(tmp_dir, relpath))
      shutil.copyfile(os.path.join(dirpath, files), os.path.join(tmp_dir, relpath, files))
if os.path.exists(os.path.join(tmp_dir, 'node', 'deps', 'v8', 'build')):
  shutil.rmtree(os.path.join(tmp_dir, 'node', 'deps', 'v8', 'build'))
if os.path.exists(os.path.join(tmp_dir, 'node', 'deps', 'v8', 'test')):   
  shutil.rmtree(os.path.join(tmp_dir, 'node', 'deps', 'v8', 'test'))
if os.path.exists(os.path.join(tmp_dir, 'node', 'deps', 'v8', 'out')):
  shutil.rmtree(os.path.join(tmp_dir, 'node', 'deps', 'v8', 'out'))
if os.path.exists(os.path.join(tmp_dir, 'node', 'deps', 'npm', 'node_modules')):
  shutil.rmtree(os.path.join(tmp_dir, 'node', 'deps', 'npm', 'node_modules'))

header_files = ['node.h', 'env.h', 'env-inl.h']
update_uvh(tmp_dir, header_files)

print 'copy file end'
print 'Begin compress file'

with tarfile.open(tarpath, 'w:gz') as tar:
  tar.add(os.path.join(tmp_dir, 'node'), arcname='node')

print 'compress end'
