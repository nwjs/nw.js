Automation
=================

## Background

The automation can replace existing automatic tests in node-webkit.

This is fundmental framework enabling users to add mocha test cases in it.

All test cases are running in seperated process so the whole test would not be blocked if some test case hang.

Each test case would output separated report in file system. It becomes easier to check the test results.

It allows to either run all test cases in batch or run each of them individually.

## How to run automation:
Assume the current directory is in "automation" folder.

1, At first time, install the npm modules:

    $npm install

2, Run all test cases

    $node mocha_test.js
    or simply call `run.sh` on Linux

3, Run individual test case

    $nw <appName>
    or
    $quiet=false nw <appName>

4, The test report will be generated under the `output` folder and sub folders named by current timestamp could be created.


