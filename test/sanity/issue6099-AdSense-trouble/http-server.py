#!/usr/bin/env python

import http.server
import socketserver
import sys

PORT = int(sys.argv[1])

Handler = http.server.SimpleHTTPRequestHandler

httpd = socketserver.TCPServer(("", PORT), Handler)

print("serving at port", PORT)
httpd.serve_forever()

