# Test with ChromeDriver {: .doctitle}
---

[TOC]

From [ChromeDriver project home page](https://sites.google.com/a/chromium.org/chromedriver/):

> WebDriver is an open source tool for automated testing of webapps across many browsers. It provides capabilities for navigating to web pages, user input, JavaScript execution, and more.  ChromeDriver is a standalone server which implements WebDriver's wire protocol for Chromium. ChromeDriver is available for Chrome on Android and Chrome on Desktop (Mac, Linux, Windows and ChromeOS).

NW.js provide a customized ChromeDriver for automated testing NW.js based apps. You can use it with tools like [selenium](http://docs.seleniumhq.org/).

## Getting started

The following workflow uses [selenium-python](http://selenium-python.readthedocs.org/) to drive the tests. You can use any language port for Selenium to work with `chromedriver`.

### Installing

* Build your application with the SDK version of nw.js from the [official] (https://nwjs.io/) website
* Install `selenium-python` in your project:
```bash
pip install selenium
```

### Running

Suppose your app shows a form for searching from remote. The page basically something like this:
```html
<form action="http://mysearch.com/search" method="GET">
    <input type="text" name="q"><input type="submit" value="Submit">
</form>
```

Write a Python script to automatically fill in the search box and submit the form:
```python
import time
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=/path/to/your/app")

driver = webdriver.Chrome(executable_path='/path/to/nwjs/chromedriver', chrome_options=chrome_options)

time.sleep(5) # Wait 5s to see the web page
search_box = driver.find_element_by_name('q')
search_box.send_keys('ChromeDriver')
search_box.submit()
time.sleep(5) # Wait 5s to see the search result
driver.quit()
```

See http://selenium-python.readthedocs.org/ for detailed documents of `selenium-python`.

## Modifications on the upstream chromedriver

* chromedriver is modified to find NW executable in the same directory by default

* An additional option `nwargs` is added if you want to pass non-switch argument to the command line:
```python
import time
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=/path/to/your/app")
chrome_options.add_experimental_option("nwargs", ["arg1", "arg2"])

driver = webdriver.Chrome(executable_path='/path/to/nwjs/chromedriver', chrome_options=chrome_options)
```
