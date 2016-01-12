#!/usr/bin/env python

import BaseHTTPServer, SimpleHTTPServer
import ssl
import sys

PORT = int(sys.argv[1])

httpd = BaseHTTPServer.HTTPServer(('localhost', PORT), SimpleHTTPServer.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket (httpd.socket, certfile='./cert.pem', keyfile='./key.pem', server_side=True)
print "serving at port", PORT
httpd.serve_forever()
