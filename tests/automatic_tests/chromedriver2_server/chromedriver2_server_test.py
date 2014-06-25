import traceback
import time
import os
from selenium import webdriver
from platform import platform

#path = os.getcwd();
#path = os.path.join(path, 'tmp-nw', 'chromedriver2_server');
path = os.path.join(os.path.abspath(__file__),"../../../tmp-nw/chromedriver2_server")
path = os.path.abspath(path)

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




driver = None
try:
    driver = webdriver.Chrome(path);
    driver.get('http://www.google.com');
    assert driver.title == 'Google'

    time.sleep(5) # Let the user actually see something!
    search_box = driver.find_element_by_name('q')
    search_box.send_keys('ChromeDriver')
    search_box.submit()
    time.sleep(5);
    title = str(driver.title[0:12])
    assert title == 'ChromeDriver'
except:
    traceback.print_exc()
else:
    print 'pass'


# Now driver.quit() will not quit normal, it is selenium's bug
# we kill process recursive, if the API works, uncomment next line and delete last 2 lines
# driver.quit()
spid = driver.service.process.pid
kill_process_tree(spid)
exit(0)
