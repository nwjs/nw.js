try{
  process.binding('stream_wrap');
  postMessage('success');
} catch (e) {
  postMessage(e.message);
}