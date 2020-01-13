#!/usr/bin/env python

import os.path as os_path
import sys

_HERE_PATH = os_path.dirname(os_path.realpath(__file__))
_SRC_PATH = os_path.normpath(os_path.join(_HERE_PATH, '..', '..', '..', '..'))
old_sys_path = sys.path[:]
sys.path.append(os_path.join(_SRC_PATH, 'third_party', 'node'))
import node, node_modules

SERVER_PATH = os_path.normpath(os_path.join(_HERE_PATH, '..', 'node_modules', 'http-server', 'bin', 'http-server'))
if sys.argv[1].isdigit():
    print SERVER_PATH

    PORT = sys.argv[1]

    node.RunNode([SERVER_PATH, '-p', PORT, '.'])
else:
    SERVER_PATH = sys.argv[1]
    print SERVER_PATH
    node.RunNode([SERVER_PATH])
    
