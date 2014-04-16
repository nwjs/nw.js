import os,re

node_version_h = os.path.join(os.path.dirname(__file__), '..', 'src',
    'nw_version.h')

f = open(node_version_h)


for line in f:
  if re.match('#define NW_MAJOR_VERSION', line):
    major = line.split()[2]
  if re.match('#define NW_MINOR_VERSION', line):
    minor = line.split()[2]
  if re.match('#define NW_PATCH_VERSION', line):
    patch = line.split()[2]

nw_version = '%(major)s.%(minor)s.%(patch)s'% locals()  

#print '%(major)s.%(minor)s.%(patch)s'% locals()




