console.log('index.js');
const logq = require('./logq.js');

(async () => {
  const test = await import('./test.mjs');
  logq.append('index.js: typeof test=' + typeof test);
  logq.append('index.js: test object=' + JSON.stringify(test));
})();
