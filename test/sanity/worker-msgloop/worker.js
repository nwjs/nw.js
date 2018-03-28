try {
const dns = require('dns');
dns.lookup('nwjs.io', () => {
    postMessage('success');
});
} catch (e) {
  postMessage(e.message);
}
