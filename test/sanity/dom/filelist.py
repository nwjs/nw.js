from unittest import TestCase
import os
import subprocess
import sys

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common import utils

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

class TestFileList(TestCase):

    def setUp(self) -> None:
        testdir = os.path.dirname(os.path.abspath(__file__))
        os.chdir(testdir)

        self.port = str(utils.free_port())
        self.server = subprocess.Popen(['python3', 'http-server.py', self.port])

        chrome_options = Options()
        chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

        self.driver = webdriver.Chrome(executable_path=os.environ["CHROMEDRIVER"], options=chrome_options)

    def test_filelist_append(self):
        try:
            self.driver.get('http://localhost:%s/index.html' % self.port)
            result_element = self.driver.find_element(By.ID, "file-input")
            result = result_element.get_attribute('innerHTML')
            assert("success" in result)
        except:
            assert(False)

    def tearDown(self) -> None:
        self.server.terminate()
        self.driver.quit()
