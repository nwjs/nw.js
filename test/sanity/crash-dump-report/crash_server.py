#!/usr/bin/env python
# Copyright 2016 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Exceptions used by the Kasko integration test module."""

import http.server
import cgi
import logging
import os
import socket
import threading
import time
import uuid
import json
import gzip
from io import StringIO, BytesIO

_LOGGER = logging.getLogger(os.path.basename(__file__))


class _StoppableHTTPServer(http.server.HTTPServer):
  """An extension of BaseHTTPServer that uses timeouts and is interruptable."""

  def server_bind(self):
    http.server.HTTPServer.server_bind(self)
    self.socket.settimeout(1)
    self.run_ = True

  def get_request(self):
    while self.run_:
      try:
        sock, addr = self.socket.accept()
        sock.settimeout(None)
        return (sock, addr)
      except socket.timeout:
        pass

  def stop(self):
    self.run_ = False

  def serve(self):
    while self.run_:
      self.handle_request()


class CrashServer(object):
  """A simple crash server for testing."""

  def __init__(self):
    self.server_ = None
    self.lock_ = threading.Lock()
    self.crashes_ = []  # Under lock_.

  def crash(self, index):
    """Accessor for the list of crashes."""
    with self.lock_:
      if index >= len(self.crashes_):
        return None
      return self.crashes_[index]

  @property
  def port(self):
    """Returns the port associated with the server."""
    if not self.server_:
      return 0
    return self.server_.server_port

  def start(self):
    """Starts the server on another thread. Call from main thread only."""
    page_handler = self.multipart_form_handler()
    self.server_ = _StoppableHTTPServer(('127.0.0.1', 0), page_handler)
    self.thread_ = self.server_thread()
    self.thread_.start()

  def stop(self):
    """Stops the running server. Call from main thread only."""
    self.server_.stop()
    self.thread_.join()
    self.server_ = None
    self.thread_ = None

  def wait_for_report(self, timeout):
    """Waits until the server has received a crash report.

    Returns True if the a report has been received in the given time, or False
    if a timeout occurred. Since Python condition variables have no notion of
    timeout this is, sadly, a busy loop on the calling thread.
    """
    started = time.time()
    elapsed = 0
    while elapsed < timeout:
      with self.lock_:
        if len(self.crashes_):
          return True
      time.sleep(0.1)
      elapsed = time.time() - started

    return False


  def multipart_form_handler(crash_server):
    """Returns a multi-part form handler class for use with a BaseHTTPServer."""

    class MultipartFormHandler(http.server.BaseHTTPRequestHandler):
      """A multi-part form handler that processes crash reports.

      This class only handles multipart form POST messages, with all other
      requests by default returning a '501 not implemented' error.
      """

      def __init__(self, request, client_address, socket_server):
        http.server.BaseHTTPRequestHandler.__init__(
          self, request, client_address, socket_server)

      def log_message(self, format, *args):
        _LOGGER.debug(format, *args)

      def do_POST(self):
        """Handles POST messages contained multipart form data."""
        content_type, parameters = cgi.parse_header(
            self.headers.get('content-type'))
        parameters['boundary'] = bytes(parameters['boundary'], "utf-8")
        if content_type != 'multipart/form-data':
          raise Exception('Unsupported Content-Type: ' + content_type)
        if self.headers.get('content-encoding') == 'gzip':
          self.log_message('GZIP');
          if self.headers.get('Transfer-Encoding', '').lower() == 'chunked':
            self.log_message('Chunked');
            if 'Content-Length' in self.headers:
              raise AssertionError
            body = self.handle_chunked_encoding()
            buffer = BytesIO(body)
          else:
            readsize = self.headers.get('content-length')
            buffer = StringIO(self.rfile.read(int(readsize)))
          with gzip.GzipFile(fileobj=buffer, mode="r") as f:
            post_multipart = cgi.parse_multipart(f, parameters)
        else:
          post_multipart = cgi.parse_multipart(self.rfile, parameters)
        self.log_message("got part")

        # Save the crash report.
        report = dict(list(post_multipart.items()))
        report_id = str(uuid.uuid4())
        report['report-id'] = [report_id]
        self.log_message("got report %s", report_id)
        self.log_message("%s", json.dumps(list(report.keys())))
        with crash_server.lock_:
          crash_server.crashes_.append(report)

        # Send the response.
        self.send_response(200)
        self.send_header("Content-Type", "text/plain")
        self.end_headers()
        self.wfile.write(report_id.encode())

      def handle_chunked_encoding(self):
        body = b''
        chunk_size = self.read_chunk_size()
        print ('chunk size: %d' % chunk_size)
        while chunk_size > 0:
          # Read the body.
          data = self.rfile.read(chunk_size)
          chunk_size -= len(data)
          body += data
          print ('chunk size: %d' % chunk_size)

          # Finished reading this chunk.
          if chunk_size == 0:
            # Read through any trailer fields.
            trailer_line = self.rfile.readline().decode()
            while trailer_line.strip() != '':
              trailer_line = self.rfile.readline().decode()
            print ('---> read line done')

            # Read the chunk size.
            chunk_size = self.read_chunk_size()
        return body

      def read_chunk_size(self):
        # Read the whole line, including the \r\n.
        chunk_size_and_ext_line = self.rfile.readline().decode('utf-8')
        # Look for a chunk extension.
        chunk_size_end = chunk_size_and_ext_line.find(';')
        if chunk_size_end == -1:
          # No chunk extensions; just encounter the end of line.
          chunk_size_end = chunk_size_and_ext_line.find('\r')
        if chunk_size_end == -1:
          self.send_response(400)  # Bad request.
          return -1
        return int(chunk_size_and_ext_line[:chunk_size_end], base=16)

    return MultipartFormHandler

  def server_thread(crash_server):
    """Returns a thread that hosts the webserver."""

    class ServerThread(threading.Thread):
      def run(self):
        crash_server.server_.serve()

    return ServerThread()

def main():
  server = CrashServer()
  server.start()

if __name__ == "__main__":
      main()
