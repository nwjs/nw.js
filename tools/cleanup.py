import argparse
import os
import shutil
import sys


def CleanUp(args):
  root_out_dir = args.root_out_dir
  if os.path.exists(root_out_dir):
    try:
        for entry in os.scandir(root_out_dir):
            if entry.is_file() and (entry.name.endswith(".breakpad") or entry.name == "nwjs.breakpad.tar"):
                file_path = entry.path
                try:
                    os.remove(file_path)
                    print(f"  Removed: {file_path}")
                except OSError as e:
                    if e.errno != errno.ENOENT:
                        print(f"  Error removing {file_path}: {e}", file=sys.stderr)
                except Exception as e:
                    print(f"  Unexpected error removing {file_path}: {e}", file=sys.stderr)

    except OSError as e:
        print(f"Error scanning directory {target_dir}: {e}", file=sys.stderr)
        return

  open(args.stamp, 'w').close()
  os.utime(args.stamp, None)

def Main():
  print(f"Executing command: {' '.join(sys.argv)}")
  parser = argparse.ArgumentParser()
  parser.add_argument(
      '--root-out-dir',
      required=True,
      help='The build output path e.g. outst/nw')
  parser.add_argument(
      '--stamp', required=True, help='Path to write the stamp file.')
  args = parser.parse_args()

  CleanUp(args)
  return 0

if __name__ == '__main__':
  sys.exit(Main())
