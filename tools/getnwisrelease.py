import sys,os,re

nw_version_h = os.path.join(os.path.dirname(__file__), '..', 'src',
    'nw_version.h')

f = open(nw_version_h)


for line in f:
  if re.match('#define NW_VERSION_IS_RELEASE', line):
    release = int(line.split()[2])
    print release
    
