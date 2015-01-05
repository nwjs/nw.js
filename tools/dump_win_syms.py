import sys, os
import subprocess

nw_exe = os.path.normpath(sys.argv[1])
sym_file = os.path.normpath(sys.argv[2])
dump_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '..', '..', 'breakpad', 'src', 'tools', 'windows', 'binaries', 'dump_syms.exe')
subprocess.call([dump_exe, nw_exe], stdout=open(sym_file, 'w'))
lzma_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '..', '..', 'third_party', 'lzma_sdk', 'Executable', '7za.exe')
archive_file = sym_file + ".7z"
subprocess.call([lzma_exe, 'a', '-t7z', archive_file, sym_file])
