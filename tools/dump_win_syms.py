import sys, os, shlex
import subprocess

nw_exe = os.path.realpath(sys.argv[1])
nw_dll = os.path.realpath(sys.argv[2])
node_dll = os.path.realpath(sys.argv[3])
out_file = os.path.realpath(sys.argv[4])
for _f in (nw_exe, nw_dll, node_dll):
    if not os.path.isfile(_f):
        raise ValueError("Input file not found: " + _f)
sym_file = os.path.realpath(nw_exe + ".sym")
dll_sym_file = os.path.realpath(nw_dll + ".sym")
node_sym_file = os.path.realpath(node_dll + ".sym")
dump_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'dump_syms.exe')
subprocess.call([dump_exe, shlex.quote(nw_exe)], stdout=open(sym_file, 'w'))
subprocess.call([dump_exe, shlex.quote(nw_dll)], stdout=open(dll_sym_file, 'w'))
subprocess.call([dump_exe, shlex.quote(node_dll)], stdout=open(node_sym_file, 'w'))
lzma_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '..', '..', 'third_party', 'lzma_sdk', 'bin', 'win64', '7za.exe')
subprocess.call([lzma_exe, 'a', '-t7z', shlex.quote(out_file), shlex.quote(sym_file), shlex.quote(dll_sym_file), shlex.quote(node_sym_file)])
