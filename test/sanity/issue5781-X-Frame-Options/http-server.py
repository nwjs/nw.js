import SocketServer
import BaseHTTPServer
import sys

PORT = int(sys.argv[1])

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    def _set_headers(s):
        s.send_response(200)
        s.send_header('Content-type', 'text/html')
        s.send_header("X-Frame-Options", "DENY")
        s.end_headers()

    def do_GET(s):
        """Respond to a GET request."""
        s._set_headers()
        s.wfile.write("<script type='text/javascript'> parent.postMessage('success', '*'); document.write('<h1 id=\"res2\">Node is ' + (typeof nw === 'undefined' ? 'DISABLED': 'ENABLED') + '</h1>');")
        s.wfile.write("</script>")

if __name__ == "__main__":
    httpd = SocketServer.TCPServer(("", PORT), MyHandler)
    httpd.serve_forever()
