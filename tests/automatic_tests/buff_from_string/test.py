#!/usr/bin/env python

from selenium import webdriver
import os
import traceback
import time
from selenium import webdriver
from platform import platform
import socket
from sys import argv

port = 13013
if len(argv) >= 2:
    port = int(argv[1])


path = os.path
dirname = path.abspath(path.dirname(__file__))
chromedriver_path = path.join(dirname,"chromedriver2_server")

nw = webdriver.Chrome(chromedriver_path,service_args=[dirname])
input_element = nw.find_element_by_id("message")
input_element.send_keys("hello world")

send_button = nw.find_element_by_id("send-message-btn")
send_button.click()

time.sleep(1)
results = nw.execute_script('return JSON.stringify(results);')

connection = socket.create_connection(("localhost",port))
connection.sendall(results)
connection.close()

def kill_process_tree(pid):
    machine_type = platform()
    if "Linux" in machine_type or "Darwin" in machine_type:
        import psutil
        parent = psutil.Process(spid)
        for child in parent.get_children(recursive=True):
            child.kill()
        parent.kill()
        return
    elif 'Windows' in machine_type:
        import subprocess
        dev_null = open(os.devnull,"wb")
        subprocess.Popen(['taskkill', '/F', '/T', '/PID', str(pid)],stdout=dev_null,stderr=dev_null)
        return
    else:
        # print "Unknow OS type"
        return

time.sleep(2)
spid = nw.service.process.pid
kill_process_tree(spid)