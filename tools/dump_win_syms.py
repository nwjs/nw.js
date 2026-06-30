import sys, os
import subprocess

def safe_open(path, mode):
  norm = os.path.normpath(path)
  if any(part == '..' for part in norm.split(os.sep)):
    raise ValueError("Path traversal detected: " + path)
  return open(norm, mode)

def safe_call(cmd, **kwargs):
  for arg in cmd:
    if isinstance(arg, str) and any(p == '..' for p in os.path.normpath(arg).split(os.sep)):
      raise ValueError("Path traversal in subprocess arg: " + arg)
  return subprocess.call(cmd, **kwargs)

nw_exe = os.path.normpath(sys.argv[1])
nw_dll = os.path.normpath(sys.argv[2])
node_dll = os.path.normpath(sys.argv[3])
out_file = os.path.normpath(sys.argv[4])
sym_file = nw_exe + ".sym"
dll_sym_file = nw_dll + ".sym"
node_sym_file = node_dll + ".sym"
dump_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'dump_syms.exe')
safe_call([dump_exe, nw_exe], stdout=safe_open(sym_file, 'w'))
safe_call([dump_exe, nw_dll], stdout=safe_open(dll_sym_file, 'w'))
safe_call([dump_exe, node_dll], stdout=safe_open(node_sym_file, 'w'))
lzma_exe = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '..', '..', 'third_party', 'lzma_sdk', 'bin', 'win64', '7za.exe')
safe_call([lzma_exe, 'a', '-t7z', out_file, sym_file, dll_sym_file, node_sym_file])
