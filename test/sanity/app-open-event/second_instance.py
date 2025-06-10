import os
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service

# 1. Set up the Options object correctly
chrome_options = Options()
testdir = os.path.dirname(os.path.abspath(__file__))

chrome_options.add_argument("nwapp=" + testdir)
chrome_options.add_argument("user-data-dir=" + os.path.join(testdir, 'userdata'))

# Assuming "success" was meant to be a command-line switch
# Use add_argument(), not the non-existent add_nw_argument()
chrome_options.add_nw_argument("success") # Prefixed with -- as is common for switches

# 2. Set up the Service object
# All service-related settings go here
try:
    chromedriver_path = os.environ['CHROMEDRIVER']
    service = Service(
        executable_path=chromedriver_path,
        log_path="log2",
        service_args=["--verbose", "--launch-timeout=5"]
    )

    # 3. Initialize the driver with the service and options objects
    driver = webdriver.Chrome(service=service, options=chrome_options)

    print("Driver initialized successfully using Selenium 4 syntax.")
    # Your code here...
    driver.quit()

except KeyError:
    print("Error: The 'CHROMEDRIVER' environment variable is not set.")
except Exception as e:
    print(f"An error occurred: {e}")
