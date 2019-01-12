# Writing Test Cases for NW.js {: .doctitle}
---

[TOC]

## Test Framework

NW.js is using a tiny Python based framework only in three files. You can read the source code in [`test` directory](https://github.com/nwjs/nw.js/tree/nw13/test).

Each test case in NW.js is a runnable app, so that you can manually run the case without the framework.

In NW.js, there are two types of test cases: `auto` and `remoting`. See [Write Test Cases](#write-test-cases) sections below to see the instructions.

To run the test cases, use the following command:

```bash
python test/test.py -t 80 auto
python test/test.py -t 80 remoting
```

## Write Test Cases

### Auto Test Case

**TODO**

### Remoting Test Case

The remoting test cases are driven by ChromeDriver. Usually these test cases involves user interaction. See [Test with ChromeDriver](../For Users/Advanced/Test with ChromeDriver.md) for the usage of ChromeDriver.

A remoting test case requires a `test.py` file in the root of the application folder.

Here is the template of `test.py` file:

```python
import time
import os

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
chrome_options = Options()
chrome_options.add_argument("nwapp=" + os.path.dirname(os.path.abspath(__file__)))

driver = webdriver.Chrome(executable_path=os.environ['CHROMEDRIVER'], chrome_options=chrome_options)
time.sleep(1)
try:
    print driver.current_url
    # Put your test code here using `assert`
finally:
    driver.quit()
```

The real case is an NW.js app. With ChromeDriver, you can simulate mouse clicks and inputs and then get the content of some DOM element. For example, with following code, when you click the button of "Click Me", a new DOM element will be added to the document with content of "success":

```html
<button id="clickme" onclick="success()">Click Me</button>
<script>
function success() {
    var el = document.createElement('div');
    el.id = 'result';
    el.innerHTML = 'success';
    document.body.appendChild(el);
}
</script>
```

Then you can test it in `test.py` script:

```python
driver.implicitly_wait(10) # 10s timeout when finding an element

clickme = driver.find_element_by_id('clickme')
clickme.click() # click the button

result = driver.find_element_by_id('result')
assert("success" in result.get_attribute('innerHTML')) # assert "success" is in the element
```
## Run Nightly Test Cases

Here are some preparations before testing.

1.Download NW SDK version.

2.Downloading selenium python library for NW with git.
The repo address and the git commit id is in [manage dependencies](https://github.com/nwjs/nw.js/blob/nw35/DEPS)(replace nw35 with the proper branch name if you are not testing 0.35).
Look for 'third_party/webdriver/pylib' in deps part of that file. The form you will see is as follows:
```js
 'src/third_party/webdriver/pylib':
    Var('chromium_git') + '/external/selenium/py.git' + '@' + 'expected commit id',
``` 
Then look for the value of Var(e.g.`chromium_git`) in vars part of that file. After you clone the pylib repo, make sure the git tip commit id is the same as expected.

3.Set environment variables.
Set `CHROMEDRIVER` to the path of chromedriver.exe in NW SDK; set `PYTHONPATH` to the 'py' folder you cloned.

4.Install Python 2.7 if needed.

5.Clone nw source repo: [nwjs](https://github.com/nwjs/nw.js).

Then you can change directory to nw repo/test; 
run ```python test.py -d <path to NW SDK> -t 60 sanity```
