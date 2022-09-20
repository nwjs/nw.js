#!/usr/bin/env python

import http.server, http.server
import ssl
import sys

PORT = int(sys.argv[1])

httpd = http.server.HTTPServer(('localhost', PORT), http.server.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket (httpd.socket, certfile='./cert.pem', keyfile='./key.pem', server_side=True)
print("serving at port", PORT)
httpd.serve_forever()
