#!/usr/bin/env python
import argparse
import boto
import datetime
import json
import os
import sys
import time
import getnwisrelease
import getnwversion


# Set timeout, for retry
#if not boto.config.has_section('Boto'):
#    boto.config.add_section('Boto')
#boto.config.set('Boto','http_socket_timeout','30')

################################
# Parse command line args
parser = argparse.ArgumentParser(description='AWS uploader, please fill in your aws key and id in Boto config (~/.boto)')
parser.add_argument('-p','--path', help='Optional. Where to find the binaries, normally out/Release/dist', required=False)
parser.add_argument('-b','--buildername', help='Builder name, e.g. linux_32bit', required=True)
parser.add_argument('-r','--revision', help='Commit revision',required=True)
parser.add_argument('-n','--number', help='Build number', required=True)
parser.add_argument('-t','--bucket', help='AWS bucket name', required=True)
parser.add_argument('-d','--dlpath', help='AWS bucket path', required=True)

args = parser.parse_args()

################################
# Check and init variables
dist_dir     = args.path
builder_name = args.buildername
got_revision = args.revision
build_number = args.number
bucket_name  = args.bucket
dlpath       = args.dlpath
date         = datetime.date.today().strftime('%m-%d-%Y')

# If the binaries location is not given, calculate it from script related dir.
if dist_dir == None:
    dist_dir = os.path.join(os.path.dirname(__file__),
            os.pardir, os.pardir, os.pardir, 'out', 'Release')

dist_dir = os.path.join(dist_dir, 'dist')

if not os.path.isabs(dist_dir):
    dist_dir = os.path.join(os.getcwd(), dist_dir)

if not os.path.isdir(dist_dir):
    print 'Invalid path: ' + dist_dir
    exit(-1)
dist_dir = os.path.normpath(dist_dir)

nw_ver = getnwversion.nw_version
if getnwisrelease.release == 0:
    nw_ver += getnwisrelease.postfix

# it's for S3, so always use '/' here
#upload_path = ''.join(['/' + date,
#                       '/' + builder_name + '-build-' + build_number + '-'  + got_revision])
upload_path = '/live-build/' + dlpath + '/v' + nw_ver;

file_list = os.listdir(dist_dir)
if len(file_list) == 0:
    print 'Cannot find packages!'
    exit(-1)

# move node-webkit- to the top of the list.
for i in range(len(file_list)):
    fname = file_list[i]
    if fname.startswith('nwjs-v') or fname.startswith('nwjs-sdk-v'):
        del file_list[i]
        file_list.insert(0,fname)
        break

def print_progress(transmitted, total):
    print ' %d%% transferred of total: %d bytes.' % (transmitted*100/total, total)
    sys.stdout.flush()


def aws_upload(upload_path, file_list):
    conn = boto.connect_s3()
    print 'Connecting to S3 ...'
    sys.stdout.flush()
    bucket = conn.get_bucket(bucket_name)
    print 'Uploading to: ' + upload_path
    for f in file_list:
        print 'Uploading "' + f + '" ...'
        sys.stdout.flush()
        # use '/' for s3
        path_prefix = ''
        if (f in ['nw.lib', 'nw.exp', 'node.lib', 'node.exp'] ) :
          if not builder_name in ['nw23_win64', 'nw23_win32'] :
              continue
          if builder_name in ['nw23_win64'] :
              path_prefix = 'x64'

        if (f.startswith('node-v') or f.startswith('nw-header') or f == 'SHASUMS256.txt') and not builder_name in ['nw23_sdk_mac64'] :
            continue

        if f.startswith('chromedriver') and 'sdk' not in builder_name :
            continue

        key = bucket.new_key(upload_path + '/' + path_prefix + '/' + f)
        key.set_contents_from_filename(filename=os.path.join(dist_dir, f), cb=print_progress, num_cb=50, replace=True)

for retry in range(3):
    try:
        aws_upload(upload_path, file_list)
        break
    except Exception, e:
        print e
        sys.stdout.flush()
        time.sleep(30) #wait for 30s and try again.

print 'Done.'

# vim: et:ts=4:sw=4
