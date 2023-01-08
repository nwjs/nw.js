#!/usr/bin/env python

import http.server, http.server
import ssl
import sys

PORT = int(sys.argv[1])

class MyHandler(http.server.BaseHTTPRequestHandler):

    def _set_headers(s):
        s.send_response(200)
        s.send_header('Content-type', 'text/html')
        s.send_header('Set-Cookie', 'same-site-cookie=foo; path=/test_cookie; SameSite=Lax; Secure')
        s.send_header('Set-Cookie', 'cross-site-cookie=bar; path=/test_cookie; SameSite=None; Secure')
        s.send_header('Set-Cookie', 'no-samesite-cookie=nee; path=/test_cookie; Secure')
        s.end_headers()

    def do_GET(s):
        """Respond to a GET request."""
        if s.path == '/test_cookie/index.html':
            cookiestring = s.headers.get('Cookie')
            log =  "Cookie sent by client on " + s.path + ": " + (cookiestring or '')
            print(log)
            s._set_headers()
            s.wfile.write(("<script type='text/javascript'> document.write('<h1 id=\\'result\\'>' + \
                          document.cookie + '</h1>'); </script> \
                          <img src='1.svg'></img> \
                          <p id='svr'>" + log + "</p>").encode())
            with open("svrlog.txt", "a") as myfile:
                myfile.write(log + "\n")
            return
        if s.path == '/test_cookie/1.svg':
            cookiestring = s.headers.get('Cookie')
            log = "Cookie sent by client on " + s.path + ": " + (cookiestring or '')
            print(log)
            with open("svrlog.txt", "a") as myfile:
                myfile.write(log + "\n")
            s.send_response(200)
            s.send_header('Content-type', 'image/svg+xml')
            s.end_headers()
            s.wfile.write(b'<svg version="1.1" baseProfile="full" height="100" width="100" xmlns="http://www.w3.org/2000/svg"> \
  <circle cx="50" cy="50" r="40" stroke="black" stroke-width="3" fill="red" /></svg>')

httpd = http.server.HTTPServer(('localhost', PORT), MyHandler)
httpd.socket = ssl.wrap_socket (httpd.socket, certfile='./cert.pem', keyfile='./key.pem', server_side=True)
print("serving at port", PORT)
httpd.serve_forever()
