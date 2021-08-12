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
from hashlib import sha256

from subprocess import call

steps = ['nw', 'symbol', 'headers', 'others']
################################
# Parse command line args
parser = argparse.ArgumentParser(description='Package nw binaries.')
parser.add_argument('-p','--path', help='Where to find the binaries, like out/Release', required=False)
parser.add_argument('-a','--arch', help='target arch', required=False)
parser.add_argument('-m','--mode', help='package mode', required=False)
parser.add_argument('-i','--icudat', help='icudat override', required=False)
group = parser.add_mutually_exclusive_group()
group.add_argument('-s','--step', choices=steps, help='Execute specified step.', required=False)
group.add_argument('-n','--skip', choices=steps, help='Skip specified step.', required=False)
args = parser.parse_args()

################################
# Init variables.
binaries_location = None        # .../out/Release
platform_name = None            # win/linux/osx
arch = None                     # ia32/x64/arm
step = None                     # nw/chromedriver/symbol
skip = None
nw_ver = None                   # x.xx
dist_dir = None                 # .../out/Release/dist
flavor = args.mode
is_headers_ok = False           # record whether nw-headers generated
package_name = 'nwjs'

if flavor in ['sdk', 'nacl']:
    package_name = 'nwjs-' + args.mode

step = args.step
skip = args.skip
binaries_location = args.path
# If the binaries location is not given, calculate it from script related dir.
if binaries_location == None:
    binaries_location = os.path.join(os.path.dirname(__file__),
            os.pardir, os.pardir, os.pardir, 'out', 'Release')

    
if not os.path.isabs(binaries_location):
    binaries_location = os.path.join(os.getcwd(), binaries_location)

if not os.path.isdir(binaries_location):
    print ('Invalid path: ' + binaries_location)
    exit(-1)
binaries_location = os.path.normpath(binaries_location)
dist_dir = os.path.join(binaries_location, 'dist')

print ('Working on ' + binaries_location)

nwfolder = os.path.join(dist_dir, '..', 'nwdist')
try:
    shutil.rmtree(nwfolder)
except:
    pass

if args.icudat != None:
    #FIXME: for some reason they are the same file (hard link) and copy will fail
    os.remove(os.path.join(binaries_location, 'icudtl.dat'))
    shutil.copy(args.icudat, binaries_location)

if sys.platform.startswith('linux'):
    platform_name = 'linux'
elif sys.platform in ('win32', 'cygwin'):
    platform_name = 'win'
elif sys.platform == 'darwin':
    platform_name = 'osx'
else:
    print ('Unsupported platform: ' + sys.platform)
    exit(-1)

_arch = platform.architecture()[0]
if _arch == '64bit':
    arch = 'x64'
elif _arch == '32bit':
    arch = 'ia32'
else:
    print ('Unsupported arch: ' + _arch)
    exit(-1)

if platform_name == 'win':
    libfile = os.path.join(binaries_location, 'nw.lib')
    libfile2 = os.path.join(binaries_location, 'node.lib')

    shutil.copy(os.path.join(binaries_location, 'nw.dll.lib'), libfile)
    shutil.copy(os.path.join(binaries_location, 'node.dll.lib'), libfile2)

if platform_name == 'win':
    arch = 'ia32'

if platform_name == 'osx':
    arch = 'x64'

if args.arch != None:
    arch = args.arch

nw_ver = getnwversion.nw_version
if getnwisrelease.release == 0:
    nw_ver += getnwisrelease.postfix

################################
# Generate targets
#
# target example:
# {
#    'input'    : [ 'nw', 'nw.pak', ... ]
#    'output'   : 'nwjs-v0.9.2-linux-x64'
#    'compress' : 'tar.gz'
#    'folder'   : True   # Optional. More than 2 files will be put into a seprate folder
#                        # normally, if you want do to this for only 1 file, set this flag.
# }
def generate_target_nw(platform_name, arch, version):
    target = {}
    # Output
    target['output'] = ''.join([
                       package_name, '-',
                       'v', version,
                       '-', platform_name,
                       '-', arch])
    target['keep4test'] = 'nwdist'
    # Compress type
    if platform_name == 'linux':
        target['compress'] = 'tar.gz'
    else:
        target['compress'] = 'zip'
    # Input
    if platform_name == 'linux':
        target['input'] = [
                           'credits.html',
                           'resources.pak',
                           'nw_100_percent.pak',
                           'nw_200_percent.pak',
                           'nw',
                           'icudtl.dat',
                           'locales',
                           'chrome_crashpad_handler',
                           'v8_context_snapshot.bin',
                           'lib/libnw.so',
                           'lib/libnode.so',
                           'lib/libffmpeg.so',
                           'lib/libEGL.so',
                           'lib/libGLESv2.so',
                           'swiftshader/libEGL.so',
                           'swiftshader/libGLESv2.so'
                           ]
        if flavor == 'sdk':
            target['input'].append('nwjc')
            target['input'].append('payload')
            target['input'].append('chromedriver')
            target['input'].append('minidump_stackwalk')
        if flavor in ['nacl','sdk'] :
            target['input'] += ['nacl_helper', 'nacl_helper_bootstrap', 'pnacl']
            if arch == 'x64':
                target['input'].append('nacl_irt_x86_64.nexe')
            elif arch == 'ia32':
                target['input'].append('nacl_irt_x86_32.nexe')
            else:
                target['input'].append('nacl_irt_{}.nexe'.format(arch))
            
    elif platform_name == 'win':
        target['input'] = [
                           'v8_context_snapshot.bin',
                           'd3dcompiler_47.dll',
                           'libEGL.dll',
                           'libGLESv2.dll',
                           'nw.dll',
                           'node.dll',
                           'nw_elf.dll',
                           'nw.exe',
                           'locales',
                           'icudtl.dat',
                           'credits.html',
                           'resources.pak',
                           'nw_100_percent.pak',
                           'nw_200_percent.pak',
                           'ffmpeg.dll',
                           'vulkan-1.dll',
                           'vk_swiftshader.dll',
                           'vk_swiftshader_icd.json',
                           'swiftshader\libEGL.dll',
                           'swiftshader\libGLESv2.dll',
                           'notification_helper.exe'
                            # To be removed in CR51
                           ]
        if flavor == 'sdk':
            target['input'].append('nwjc.exe')
            target['input'].append('payload.exe')
            target['input'].append('chromedriver.exe')
        if flavor in ['nacl','sdk'] :
            target['input'].append('pnacl')
            target['input'].append('nacl_irt_x86_64.nexe')
            if arch == 'ia32':
                target['input'].append('nacl_irt_x86_32.nexe')
                target['input'].append('nacl64.exe')
    elif platform_name == 'osx':
        target['input'] = [
                           'nwjs.app',
                           'credits.html',
                          ]
        if flavor == 'sdk':
            target['input'].append('nwjc')
            target['input'].append('payload')
            target['input'].append('chromedriver')
            target['input'].append('libffmpeg.dylib')
            target['input'].append('minidump_stackwalk')
            target['input'].append('v8_context_snapshot.x86_64.bin')
    else:
        print ('Unsupported platform: ' + platform_name)
        exit(-1)
    return target

def generate_target_chromedriver(platform_name, arch, version):
    if args.mode != 'sdk':
        return generate_target_empty(platform_name, arch, version)

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
    target['output'] = ''.join([package_name, '-symbol-',
                                'v', version,
                                '-', platform_name,
                                '-', arch])
    if platform_name == 'linux':
        target['compress'] = 'tar.gz'
        target['input'] = [
            'nw.sym',
            'libnode.sym',
            'libnw.sym',
            'libffmpeg.sym'
        ]
        if flavor in ['sdk', 'nacl']:
            target['input'].append('nacl_helper.sym')

        target['folder'] = True
    elif platform_name == 'win':
        target['compress'] = None
        target['input'] = ['nw.sym.7z']
        target['output'] = ''.join([package_name, '-symbol-',
                                    'v', version,
                                    '-', platform_name,
                                    '-', arch, '.7z'])
    elif platform_name == 'osx':
        target['compress'] = 'zip'
        target['input'] = [
                          'nwjs.breakpad.tar'
                          ]
        target['folder'] = True
    else:
        print ('Unsupported platform: ' + platform_name)
        exit(-1)
    return target

def generate_target_headers(platform_name, arch, version):
    # here, call make_nw_header tool to generate headers
    # then, move to binaries_location
    target = {}
    target['output'] = ''
    target['compress'] = None
    target['input'] = []
    # here , call make-nw-headers.py to generate nw headers
    make_nw_header = os.path.join(os.path.dirname(__file__), \
                                  'make-nw-headers.py')
    print (make_nw_header)
    res = call(['python', make_nw_header, '-p', binaries_location, '-n', platform_name])
    if res == 0:
        print ('nw-headers generated')
        nw_headers_name = 'nw-headers-v' + version + '.tar.gz'
        nw_headers_path = os.path.join(os.path.dirname(__file__), \
                                       os.pardir, 'tmp', nw_headers_name)
        if os.path.isfile(os.path.join(binaries_location, nw_headers_name)):
            os.remove(os.path.join(binaries_location, nw_headers_name))

        f = open(nw_headers_path, 'rb')
        checksum_file = open(os.path.join(binaries_location, 'SHASUMS256.txt'), 'w')
        with f, checksum_file:
            checksum_file.write('%s %s' % (sha256(f.read()).hexdigest(), nw_headers_name))
        shutil.move(nw_headers_path, binaries_location)
        target['input'].append(nw_headers_name)
        target['input'].append('SHASUMS256.txt')
    else:
        #TODO, handle err
        print ('nw-headers generate failed')
    return target

def generate_target_empty(platform_name, arch, version):
    target = {}
    target['output'] = ''
    target['compress'] = None
    if platform_name == 'win':
        target['input'] = []
    elif platform_name == 'linux' :
        target['input'] = []
    else:
        target['input'] = []
    return target

def generate_target_others(platform_name, arch, version):
    target = {}
    target['output'] = ''
    target['compress'] = None
    if platform_name == 'win':
        target['input'] = ['nw.lib', 'node.lib']
    elif platform_name == 'linux' :
        target['input'] = []
    else:
        target['input'] = []
    return target


################################
# Make packages

#https://gist.github.com/kgn/610907
def ZipDir(inputDir, outputZip):
    '''Zip up a directory and preserve symlinks and empty directories'''
    zipOut = zipfile.ZipFile(outputZip, 'w', compression=zipfile.ZIP_DEFLATED)
    
    rootLen = len(os.path.dirname(inputDir))
    def _ArchiveDirectory(parentDirectory):
        contents = os.listdir(parentDirectory)
        #store empty directories
        if not contents:
            #http://www.velocityreviews.com/forums/t318840-add-empty-directory-using-zipfile.html
            archiveRoot = parentDirectory[rootLen:].replace('\\', '/').lstrip('/')
            zipInfo = zipfile.ZipInfo(archiveRoot+'/')
            zipOut.writestr(zipInfo, '')
        for item in contents:
            fullPath = os.path.join(parentDirectory, item)
            if os.path.isdir(fullPath) and not os.path.islink(fullPath):
                _ArchiveDirectory(fullPath)
            else:
                archiveRoot = fullPath[rootLen:].replace('\\', '/').lstrip('/')
                if os.path.islink(fullPath):
                    # http://www.mail-archive.com/python-list@python.org/msg34223.html
                    zipInfo = zipfile.ZipInfo(archiveRoot)
                    zipInfo.create_system = 3
                    # long type of hex val of '0xA1ED0000L',
                    # say, symlink attr magic...
                    zipInfo.external_attr = 2716663808
                    zipOut.writestr(zipInfo, os.readlink(fullPath))
                else:
                    zipOut.write(fullPath, archiveRoot, zipfile.ZIP_DEFLATED)
    _ArchiveDirectory(inputDir)
    
    zipOut.close()

def compress(from_dir, to_dir, fname, compress):
    from_dir = os.path.normpath(from_dir)
    to_dir = os.path.normpath(to_dir)
    _from = os.path.join(from_dir, fname)
    _to = os.path.join(to_dir, fname)
    if compress == 'zip':
        if os.path.isdir(_from): 
            ZipDir(_from, _to + '.zip')
        else:
            z = zipfile.ZipFile(_to + '.zip', 'w', compression=zipfile.ZIP_DEFLATED)
            z.write(_from, fname)
            z.close()
    elif compress == 'tar.gz': # only for folders
        if not os.path.isdir(_from):
            print ('Will not create tar.gz for a single file: ' + _from)
            exit(-1)
        with tarfile.open(_to + '.tar.gz', 'w:gz') as tar:
            tar.add(_from, arcname=os.path.basename(_from))
    elif compress == 'gz': # only for single file
        if os.path.isdir(_from):
            print ('Will not create gz for a folder: ' + _from)
            exit(-1)
        f_in = open(_from, 'rb')
        f_out = gzip.open(_to + '.gz', 'wb')
        f_out.writelines(f_in)
        f_out.close()
        f_in.close()
    else:
        print ('Unsupported compression format: ' + compress)
        exit(-1)


def make_packages(targets):
    # check file existance
    for t in targets:
        for f in t['input']:
            src = os.path.join(binaries_location, f)
            if not os.path.exists(src):
                print ('File does not exist: ', src)
                exit(-1)

    # clear the output folder
    if os.path.exists(dist_dir):
        if not os.path.isdir(dist_dir):
            print ('Invalid path: ' + dist_dir)
            exit(-1)
        else:
            shutil.rmtree(dist_dir)

    # now let's do it
    os.mkdir(dist_dir)
    for t in targets:
        if len(t['input']) == 0:
            continue
        if t['compress'] == None:
            for f in t['input']:
                src = os.path.join(binaries_location, f)
                if t['output'] != '':
                    dest = os.path.join(dist_dir, t['output'])
                else:
                    dest = os.path.join(dist_dir, f)
                print ("Copying " + f)
                shutil.copy(src, dest)
        elif ('folder' in t and t['folder'] == True) or len(t['input']) > 1:
            print ('Making "' + t['output'] + '.' + t['compress'] + '"')
            # copy files into a folder then pack
            folder = os.path.join(dist_dir, t['output'])
            os.mkdir(folder)
            if platform_name == 'linux':
                os.mkdir(os.path.join(folder, 'lib'))
            if platform_name in ['linux', 'win']:
                os.mkdir(os.path.join(folder, 'swiftshader'))
            for f in t['input']:
                src = os.path.join(binaries_location, f)
                dest = os.path.join(folder, f)
                if os.path.isdir(src): # like nw.app
                    shutil.copytree(src, dest, symlinks=True)
                else:
                    shutil.copy(src, dest)
            compress(dist_dir, dist_dir, t['output'], t['compress'])
            # remove temp folders
            if ('keep4test' in t) :
                shutil.copytree(folder, nwfolder, symlinks=True)
            
            shutil.rmtree(folder)
        else:
            # single file
            print ('Making "' + t['output'] + '.' + t['compress'] + '"')
            compress(binaries_location, dist_dir, t['input'][0], t['compress'])

# must be aligned with steps
generators = {}
generators['nw'] = generate_target_nw
generators['chromedriver'] = generate_target_chromedriver
generators['symbol'] = generate_target_symbols
generators['headers'] = generate_target_headers
generators['others'] = generate_target_others
################################
# Process targets
targets = []
for s in steps:
    if (step != None) and (s != step):
            continue
    if (skip != None) and (s == skip):
            continue
    targets.append(generators[s](platform_name, arch, nw_ver))

print ('Creating packages...')
make_packages(targets)

# vim: et:ts=4:sw=4
