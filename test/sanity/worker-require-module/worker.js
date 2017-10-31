try {
  postMessage(require('test'));
} catch (e) {
  postMessage(e.message);
}