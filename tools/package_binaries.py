#!/usr/bin/env python
import os
import shutil
import tarfile
import sys


script_dir = os.path.dirname(__file__)
nw_root  = os.path.normpath(os.path.join(script_dir, os.pardir))
project_root = os.path.normpath(os.path.join(nw_root, os.pardir, os.pardir)); 
#default project path
project_root = os.path.join(project_root, 'out', 'Release')
required_file = []
tarname = []
binary_name = []
binary_tar = []
binary_store_path = []
binary_full_path = []
binary_tar_full_path = []

#parse command line arguments
"""
-p <path>, the absolute path of executable files
""" 
if '-p' in sys.argv:
  tmp = sys.argv[sys.argv.index('-p') + 1]
  if not os.path.isabs(tmp):
    print 'the path is not an absolute path.\n'
    exit()

  if not os.path.exists(tmp):
    print 'the directory does not exist.\n'
    exit()
        
  project_root = tmp



#get platform information
if sys.platform.startswith('linux'):
  platform_name = 'linux'
  
if sys.platform in ('win32', 'cygwin'):
  platform_name = 'win'
  
if sys.platform == 'darwin':
  platform_name = 'osx'

#judge whether the target exist or not   
if platform_name == 'linux' and not os.path.exists(
    os.path.join(project_root, 'nw')):
  print 'nw file does not exist.\n'
  exit()  

if platform_name == 'win' and not os.path.exists(
    os.path.join(project_root, 'nw.exe')):
  print 'nw file does not exist.\n'
  exit() 
  
if platform_name == 'osx' and not os.path.exists(
    os.path.join(project_root, 'node-webkit.app')):
  print 'nw file does not exist.\n'
  exit()

if platform_name != 'win' and not os.path.exists(
  os.path.join(project_root, 'chromedriver2_server')):
  print 'chromedriver2_server file does not exist.\n'
  exit()

if platform_name == 'win' and not os.path.exists(
  os.path.join(project_root, 'chromedriver2_server.exe')):
  print 'chromedriver2_server file does not exist.\n'
  exit()


required_file_linux = (
  'nw',
  'nw.pak',
  'libffmpegsumo.so',
  'nwsnapshot',
  'credits.html',
)

required_file_win = (
  'ffmpegsumo.dll',
  'icudt.dll',
  'libEGL.dll',
  'libGLESv2.dll',
  'nw.exe',
  'nw.pak',
  'nwsnapshot.exe',
  'credits.html',
)

required_file_mac = (
  'node-webkit.app',
  'nwsnapshot',
  'credits.html',
)

required_chromedriver2_file_win = (
  'chromedriver2_server.exe',
)

required_chromedriver2_file_others = (
  'chromedriver2_server',
)

if (platform_name == 'linux'):
  required_file.append(required_file_linux)
  required_file.append(required_chromedriver2_file_others)

if (platform_name == 'win'):
  required_file.append(required_file_win)
  required_file.append(required_chromedriver2_file_win);

if (platform_name == 'osx'):
  required_file.append(required_file_mac)
  required_file.append(required_chromedriver2_file_others)


#generate binary tar name
import getnwisrelease
import getnwversion
import platform

nw_version = 'v' + getnwversion.nw_version
is_release = getnwisrelease.release
if is_release == 0:
  nw_version += getnwisrelease.postfix

bits = platform.architecture()[0]

if bits == '64bit':
  arch = 'x64'
else:
  arch = 'ia32'

if (platform_name == 'win' or platform_name == 'osx'):
  arch = 'ia32'

tarname.append('node-webkit-' + nw_version)
tarname.append('chromedriver2-nw-' + nw_version)
for index in range(len(tarname)):
  binary_name.append(tarname[index] + '-' + platform_name + '-' + arch);
  binary_tar.append(binary_name[index] + '.tar.gz')

#use zip in mac and windows
if platform_name in ('win', 'osx'):
  for index in range(len(binary_name)):
    binary_tar[index] = binary_name[index] + '.zip'


#make directory for binary_tar
binary_store_path.append(os.path.join(project_root,                                 
                                 'node-webkit-binaries'))
binary_store_path.append(os.path.join(project_root,                                 
                                 'chromedriver2-binaries'))

for index in range(len(binary_store_path)):
  if not os.path.exists(binary_store_path[index]):
    os.mkdir(binary_store_path[index])

for index in range(len(binary_store_path)):
  binary_full_path.append(os.path.join(binary_store_path[index], binary_name[index]))
  binary_tar_full_path.append(os.path.join(binary_store_path[index], binary_tar[index]))

for index in range(len(binary_full_path)):
  if os.path.exists(binary_full_path[index]):
    shutil.rmtree(binary_full_path[index])

for index in range(len(binary_full_path)):
  os.mkdir(binary_full_path[index])


#copy file to binary
print 'Begin copy file.'
for index in range(len(required_file)):
  for file in required_file[index]:
    try:
      shutil.copy(os.path.join(project_root, file),
                  os.path.join(binary_full_path[index], file))
    except:
      shutil.copytree(os.path.join(project_root, file),
                      os.path.join(binary_full_path[index], file))
    
print 'copy file end.\n'

for index in range(len(binary_tar_full_path)):
  if (os.path.isfile(binary_tar_full_path[index])):
    os.remove(binary_tar_full_path[index])
  
print 'Begin compress file'

if platform_name in ('win', 'osx'):
  """
  If there are sub directors, this should be modified  
  """
  import zipfile
  for index in range(len(binary_tar_full_path)):                     
    zip = zipfile.ZipFile(binary_tar_full_path[index], 'w',
                        compression=zipfile.ZIP_DEFLATED)

    for dirpath, dirnames, filenames in os.walk(binary_full_path[index]):
      for name in filenames:
        path = os.path.normpath(os.path.join(dirpath, name))
        zip.write(path, path.replace(binary_full_path[index] + os.sep, ''))

    zip.close()

else:
  for index in range(len(binary_tar_full_path)):                       
    tar = tarfile.open(binary_tar_full_path[index], 'w:gz')
    tar.add(binary_full_path[index], os.path.basename(binary_full_path[index]))
    tar.close()

  
print 'compress file end.\n'

for index in range(len(binary_store_path)):
  print 'the binaries files store in path:', os.path.normpath(
      os.path.join(os.getcwd(), binary_store_path[index]))


