#!/usr/bin/env python
import argparse
import getnwisrelease
import getnwversion
import gzip
import os
import platform
import shutil
import sys
import tarfile
import zipfile

################################
# Parse command line args
parser = argparse.ArgumentParser(description='Package nw binaries.')
parser.add_argument('-p','--path', help='Where to find the binaries, like out/Release', required=False)
parser.add_argument('-s','--step', help='Execute specified step. (could be "nw", "chromedriver" or "symbol")', required=False)
args = parser.parse_args()

################################
# Init variables.
binaries_location = None        # .../out/Release
platform_name = None            # win/linux/osx
arch = None                     # ia32/x64
step = None                     # nw/chromedriver/symbol
nw_ver = None                   # x.xx
dist_dir = None                 # .../out/Release/dist

step = args.step
binaries_location = args.path
# If the binaries location is not given, calculate it from script related dir.
if binaries_location == None:
    binaries_location = os.path.join(os.path.dirname(__file__),
            os.pardir, os.pardir, os.pardir, 'out', 'Release')

if not os.path.isabs(binaries_location):
    binaries_location = os.path.join(os.getcwd(), binaries_location)

if not os.path.isdir(binaries_location):
    print 'Invalid path: ' + binaries_location
    exit(-1)
binaries_location = os.path.normpath(binaries_location)
dist_dir = os.path.join(binaries_location, 'dist')

print 'Working on ' + binaries_location

if sys.platform.startswith('linux'):
    platform_name = 'linux'
elif sys.platform in ('win32', 'cygwin'):
    platform_name = 'win'
elif sys.platform == 'darwin':
    platform_name = 'osx'
else:
    print 'Unsupported platform: ' + sys.platform
    exit(-1)

_arch = platform.architecture()[0]
if _arch == '64bit':
    arch = 'x64'
elif _arch == '32bit':
    arch = 'ia32'
else:
    print 'Unsupported arch: ' + _arch
    exit(-1)

if platform_name == 'win' or platform_name == 'osx':
    arch = 'ia32'

nw_ver = getnwversion.nw_version
if getnwisrelease.release == 0:
    nw_ver += getnwisrelease.postfix

################################
# Generate targets
#
# target example:
# {
#    'input'    : [ 'nw', 'nw.pak', ... ]
#    'output'   : 'node-webkit-v0.9.2-linux-x64'
#    'compress' : 'tar.gz'
#    'folder'   : True   # Optional. More than 2 files will be put into a seprate folder
#                        # normally, if you want do to this for only 1 file, set this flag.
# }
def generate_target_nw(platform_name, arch, version):
    target = {}
    # Output
    target['output'] = ''.join([
                       'node-webkit-',
                       'v', version,
                       '-', platform_name,
                       '-', arch])
    # Compress type
    if platform_name == 'linux':
        target['compress'] = 'tar.gz'
    else:
        target['compress'] = 'zip'
    # Input
    if platform_name == 'linux':
        target['input'] = [
                           'credits.html',
                           'libffmpegsumo.so',
                           'nw.pak',
                           'nwsnapshot',
                           'nw',
                           ]
    elif platform_name == 'win':
        target['input'] = [
                           'ffmpegsumo.dll',
                           'icudt.dll',
                           'libEGL.dll',
                           'libGLESv2.dll',
                           'nw.exe',
                           'nw.pak',
                           'nw.exp',
                           'nw.lib',
                           'nwsnapshot.exe',
                           'credits.html',
                           ]
    elif platform_name == 'osx':
        target['input'] = [
                           'node-webkit.app',
                           'nwsnapshot',
                           'credits.html',
                          ]
    else:
        print 'Unsupported platform: ' + platform_name
        exit(-1)
    return target

def generate_target_chromedriver(platform_name, arch, version):
    target = {}
    # Output
    target['output'] = ''.join([
                       'chromedriver-nw-',
                       'v', version,
                       '-', platform_name,
                       '-', arch])
    # Compress type
    if platform_name == 'linux':
        target['compress'] = 'tar.gz'
    else:
        target['compress'] = 'zip'
    # Input
    if platform_name == 'win':
        target['input'] = ['chromedriver.exe']
    else:
        target['input'] = ['chromedriver']
    target['folder'] = True # always create a folder
    return target

def generate_target_symbols(platform_name, arch, version):
    target = {}
    target['output'] = ''.join(['node-webkit-symbol-',
                                'v', version,
                                '-', platform_name,
                                '-', arch])
    if platform_name == 'linux':
        target['compress'] = 'tar.gz'
        target['input'] = ['nw.breakpad.' + arch]
    elif platform_name == 'win':
        target['compress'] = 'zip'
        target['input'] = ['nw.exe.pdb']
    elif platform_name == 'osx':
        target['compress'] = 'tar.gz'
        target['input'] = [
                          'node-webkit.app.dSYM',
                          'node-webkit Helper.app.dSYM',
                          'node-webkit Framework.framework.dSYM',
                          'ffmpegsumo.so.dSYM',
                          ]
    else:
        print 'Unsupported platform: ' + platform_name
        exit(-1)
    target['folder'] = True
    return target


################################
# Make packages
def compress(from_dir, to_dir, fname, compress):
    from_dir = os.path.normpath(from_dir)
    to_dir = os.path.normpath(to_dir)
    _from = os.path.join(from_dir, fname)
    _to = os.path.join(to_dir, fname)
    if compress == 'zip':
        z = zipfile.ZipFile(_to + '.zip', 'w', compression=zipfile.ZIP_DEFLATED)
        if os.path.isdir(_from):
            for root, dirs, files in os.walk(_from):
                for f in files:
                    _path = os.path.join(root, f)
                    z.write(_path, _path.replace(from_dir+os.sep, ''))
        else:
            z.write(_from, fname)
        z.close()
    elif compress == 'tar.gz': # only for folders
        if not os.path.isdir(_from):
            print 'Will not create tar.gz for a single file: ' + _from
            exit(-1)
        with tarfile.open(_to + '.tar.gz', 'w:gz') as tar:
            tar.add(_from, arcname=os.path.basename(_from))
    elif compress == 'gz': # only for single file
        if os.path.isdir(_from):
            print 'Will not create gz for a folder: ' + _from
            exit(-1)
        f_in = open(_from, 'rb')
        f_out = gzip.open(_to + '.gz', 'wb')
        f_out.writelines(f_in)
        f_out.close()
        f_in.close()
    else:
        print 'Unsupported compression format: ' + compress
        exit(-1)


def make_packages(targets):

    # check file existance
    for t in targets:
        for f in t['input']:
            src = os.path.join(binaries_location, f)
            if not os.path.exists(src):
                print 'File does not exist: ', src
                exit(-1)

    # clear the output folder
    if os.path.exists(dist_dir):
        if not os.path.isdir(dist_dir):
            print 'Invalid path: ' + dist_dir
            exit(-1)
        else:
            shutil.rmtree(dist_dir)

    # now let's do it
    os.mkdir(dist_dir)
    for t in targets:
        print 'Making "' + t['output'] + '.' + t['compress'] + '"'
        if (t.has_key('folder') and t['folder'] == True) or len(t['input']) > 1:
            # copy files into a folder then pack
            folder = os.path.join(dist_dir, t['output'])
            os.mkdir(folder)
            for f in t['input']:
                src = os.path.join(binaries_location, f)
                dest = os.path.join(folder, f)
                if os.path.isdir(src): # like nw.app
                    shutil.copytree(src, dest)
                else:
                    shutil.copy(src, dest)
            compress(dist_dir, dist_dir, t['output'], t['compress'])
            # remove temp folders
            shutil.rmtree(folder)
        else:
            # single file
            compress(binaries_location, dist_dir, t['input'][0], t['compress'])


################################
# Process targets
targets = []
if step == 'nw':
    targets.append(generate_target_nw(platform_name, arch, nw_ver))
elif step == 'chromedriver':
    targets.append(generate_target_chromedriver(platform_name, arch, nw_ver))
elif step == 'symbol':
    targets.append(generate_target_symbols(platform_name, arch, nw_ver))
else:
    targets.append(generate_target_nw(platform_name, arch, nw_ver))
    targets.append(generate_target_chromedriver(platform_name, arch, nw_ver))
    targets.append(generate_target_symbols(platform_name, arch, nw_ver))

print 'Creating packages...'
make_packages(targets)

# vim: et:ts=4:sw=4
