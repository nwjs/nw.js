#!/usr/bin/env python
import argparse
import boto3
import datetime
import json
import os
import sys
import time
import re
import getnwisrelease
import getnwversion

from boto3.s3.transfer import TransferConfig

# Set timeout, for retry
#if not boto.config.has_section('Boto'):
#    boto.config.add_section('Boto')
#boto.config.set('Boto','http_socket_timeout','30')

################################
# Parse command line args
parser = argparse.ArgumentParser(description='AWS uploader, please fill in your aws key and id in Boto config (~/.boto)')
parser.add_argument('-p','--path', help='Optional. Where to find the binaries, normally out/Release/dist', required=False)
parser.add_argument('-b','--buildername', help='Builder name, e.g. linux_32bit', required=True)
parser.add_argument('-r','--revision', help='Commit revision',required=False)
parser.add_argument('-n','--number', help='Build number', required=False)
parser.add_argument('-t','--bucket', help='AWS bucket name', required=True)
parser.add_argument('-d','--dlpath', help='AWS bucket path', required=True)
parser.add_argument('--header', help='upload header', action="store_true")

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
    print ('Invalid path: ' + dist_dir)
    exit(-1)
dist_dir = os.path.normpath(dist_dir)

nw_ver = getnwversion.nw_version
if getnwisrelease.release == 0:
    nw_ver += getnwisrelease.postfix

# it's for S3, so always use '/' here
#upload_path = ''.join(['/' + date,
#                       '/' + builder_name + '-build-' + build_number + '-'  + got_revision])
upload_path = 'live-build/' + dlpath;

file_list = os.listdir(dist_dir)
if len(file_list) == 0:
    print ('Cannot find packages!')
    exit(-1)

# move node-webkit- to the top of the list.
for i in range(len(file_list)):
    fname = file_list[i]
    if fname.startswith('nwjs-v') or fname.startswith('nwjs-sdk-v'):
        del file_list[i]
        file_list.insert(0,fname)
        break

def print_progress(transmitted, total):
    print (' %d%% transferred of total: %d bytes.' % (transmitted*100/total, total))
    sys.stdout.flush()


def aws_upload(upload_path, file_list):
    print ('Connecting to S3 ...')
    sys.stdout.flush()
    session = boto3.Session()
    s3_client = session.client('s3')
    print ('Uploading to: ' + upload_path)
    win_non_sdk = re.compile('nw[a-zA-Z0-9]+_win')
    for f in file_list:
        print ('Uploading "' + f + '" ...')
        sys.stdout.flush()
        # use '/' for s3
        path_prefix = ''
        if (f in ['nw.lib', 'nw.exp', 'node.lib', 'node.exp',
                  'nw.lib.sha256.txt', 'nw.exp.sha256.txt',
                  'node.lib.sha256.txt', 'node.exp.sha256.txt'] ) :
          if not win_non_sdk.match(builder_name) :
              continue
          if 'win64' in builder_name :
              path_prefix = 'x64'
          if 'winarm' in builder_name :
              path_prefix = 'arm64'

        if (f == 'SHASUMS256.txt'):
            continue

        if f.startswith('nw-header') and not ('_sdk_mac64' in builder_name) and not args.header :
            continue

        if f.startswith('node-v')  and not ('_sdk_mac64' in builder_name) :
            continue

        if f.startswith('chromedriver') and 'sdk' not in builder_name :
            continue

        parts = [upload_path, path_prefix, f]
        key = "/".join(part for part in parts if part)

        local_file_path = os.path.join(dist_dir, f)
        s3_client.upload_file(local_file_path, bucket_name, key)

for retry in range(3):
    try:
        aws_upload(upload_path, file_list)
        break
    except (Exception, e):
        print (e)
        sys.stdout.flush()
        time.sleep(30) #wait for 30s and try again.

print ('Done.')

# vim: et:ts=4:sw=4
