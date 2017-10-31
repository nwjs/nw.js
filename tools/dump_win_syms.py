import sys, os
import subprocess

nw_exe = os.path.normpath(sys.argv[1])
nw_dll = os.path.normpath(sys.argv[2])
node_dll = os.path.normpath(sys.argv[3])
out_file = os.path.normpath(sys.argv[4])
sym_file = nw_exe + ".sym"
dll_sym_file = nw_dll + ".sym"
node_sym_file = node_dll + ".sym"
dump_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'dump_syms.exe')
subprocess.call([dump_exe, nw_exe], stdout=open(sym_file, 'w'))
subprocess.call([dump_exe, nw_dll], stdout=open(dll_sym_file, 'w'))
subprocess.call([dump_exe, node_dll], stdout=open(node_sym_file, 'w'))
lzma_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '..', '..', 'third_party', 'lzma_sdk', 'Executable', '7za.exe')
subprocess.call([lzma_exe, 'a', '-t7z', out_file, sym_file, dll_sym_file, node_sym_file])
