#!/usr/bin/env python

import os.path as os_path

_HERE_PATH = os_path.dirname(os_path.realpath(__file__))
_SRC_PATH = os_path.normpath(os_path.join(_HERE_PATH, '..', '..', '..', '..'))
SERVER_PATH = os_path.normpath(os_path.join(_HERE_PATH, '..', 'node_modules', 'http-server', 'bin', 'http-server'))
print SERVER_PATH

import sys
PORT = sys.argv[1]
old_sys_path = sys.path[:]
sys.path.append(os_path.join(_SRC_PATH, 'third_party', 'node'))
import node, node_modules

node.RunNode([SERVER_PATH, '-p', PORT, '.'])
