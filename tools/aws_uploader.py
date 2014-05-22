#!/usr/bin/env python
import argparse
import boto
import datetime
import json
import os

################################
# Parse command line args
parser = argparse.ArgumentParser(description='AWS uploader.')
parser.add_argument('-p','--path', help='Optional. Where to find the binaries, normally out/Release/dist', required=False)
parser.add_argument('-b','--buildername', help='Builder name, e.g. linux_32bit', required=True)
parser.add_argument('-r','--revision', help='Commit revision',required=True)
parser.add_argument('-n','--number', help='Build number', required=True)
parser.add_argument('-t','--bucket', help='AWS bucket name', required=True)
parser.add_argument('-k','--keyfile', help='Optional. JSON file containing AWS access id and key, default: ~/.awskey; ' +\
        'The content of the key file is like {"awsid":"ABCDEF","awskey":"123456"}', required=False)

args = parser.parse_args()

################################
# Check and init variables
dist_dir     = args.path
builder_name = args.buildername
got_revision = args.revision
build_number = args.number
bucket_name  = args.bucket
keyfile      = args.keyfile
date         = datetime.date.today().strftime('%m-%d-%Y')

# If the binaries location is not given, calculate it from script related dir.
if dist_dir == None:
    dist_dir = os.path.join(os.path.dirname(__file__),
            os.pardir, os.pardir, os.pardir, 'out', 'Release', 'dist')

if not os.path.isabs(dist_dir):
    dist_dir = os.path.join(os.getcwd(), dist_dir)

if not os.path.isdir(dist_dir):
    print 'Invalid path: ' + dist_dir
    exit(-1)
dist_dir = os.path.normpath(dist_dir)

# Check aws keyfile
if keyfile == None:
    keyfile = os.path.join(os.path.expanduser('~'),'.awskey')
    print keyfile
if not os.path.exists(keyfile):
    print "Cannot find aws key file"
    exit(-1)

json_data = open(keyfile)
data = json.load(json_data)
if not (data.has_key('awsid') and data.has_key('awskey')):
    print "Invalid key file format."
    exit(-1)
awsid = data['awsid']
awskey = data['awskey']

# it's for S3, so always use '/' here
upload_path = ''.join(['/' + date,
                       '/' + builder_name + '-build-' + build_number + '-'  + got_revision])

file_list = os.listdir(dist_dir)
if len(file_list) == 0:
    print 'Cannot find packages!'
    exit(-1)


def print_progress(transmitted, total):
    print ' %d%% transferred of total: %d bytes.' % (transmitted*100/total, total)


def aws_upload(upload_path, file_list):
    conn = boto.connect_s3(awsid, awskey)
    print 'Connecting to S3 ...'
    bucket = conn.get_bucket(bucket_name)
    print 'Uploading to: ' + upload_path
    for f in file_list:
        print 'Uploading "' + f + '" ...'
        # use '/' for s3
        key = bucket.new_key(upload_path + '/' + f)
        key.set_contents_from_filename(filename=os.path.join(dist_dir, f), cb=print_progress, num_cb=50, replace=True)

aws_upload(upload_path, file_list)
print 'Done.'


# vim: et:ts=4:sw=4
