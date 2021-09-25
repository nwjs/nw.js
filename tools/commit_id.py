import subprocess as sp
import sys
import os
import traceback

# Usage: commit_id.py check <root_src_dir> (checks if git is present)
# Usage: commit_id.py gen <root_src_dir> <file_to_write> (generates commit id)

def grab_output(command, cwd):
    with sp.Popen(command, stdout=sp.PIPE, shell=True, cwd=cwd) as p:
        output, errors = p.communicate()
    return output.decode('utf-8').splitlines()[0].strip()

operation = sys.argv[1]
cwd = sys.argv[2]
repos = ['content/nw', '.', 'v8', 'third_party/node-nw']

if operation == 'check':
    for repo in repos:
        index_path = os.path.join(cwd, repo, '.git', 'index')
        if not os.path.exists(index_path):
            print("0")
            sys.exit(0)
    print("1")
    sys.exit(0)

output_file = sys.argv[3]
commit_id_size = 7
final_hash = ''

for repo in repos:
    try:
        repo_path = os.path.join(cwd, repo)
        print("repo: %s\n" % repo_path)
        final_hash += grab_output('git rev-parse --short=%d HEAD' % commit_id_size, repo_path)
        final_hash += '-'
    except Exception as e:
        traceback.print_exc()
        final_hash = 'invalid-hash'
        break

final_hash = final_hash.rstrip('-')
hfile = open(output_file, 'w')

hfile.write('#define NW_COMMIT_HASH "%s"\n'    % final_hash)
hfile.write('#define NW_COMMIT_HASH_SIZE %d\n' % len(final_hash))

hfile.close()
