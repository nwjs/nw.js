#!/usr/bin/env python

import http.server
import socketserver
import sys

PORT = 0

Handler = http.server.SimpleHTTPRequestHandler

httpd = socketserver.TCPServer(("", PORT), Handler)
actual_port = httpd.server_address[1]
print(actual_port)
sys.stdout.flush()
#print("serving at port ", actual_port)
httpd.serve_forever()

