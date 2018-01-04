import {TestModule} from "./lib.js";

var f = new TestModule();
document.getElementById('result').innerHTML = 'result: ' + f.foo(10);
document.getElementById('source').innerHTML = "f.foo source: " + f.foo.toString();
