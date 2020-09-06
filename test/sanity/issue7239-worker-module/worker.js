onmessage = (event) => {
  try {
    const addon = require('./build/Release/buffer_example');

    const buffer = Buffer.from(event.data);

    for (var i = 0; i < 1000; i++) {
      var b = Buffer.from("CDE");
      addon.rotate(b, i);
    }

    // synchronous, rotates each character by +13
    var result = addon.rotate(buffer, 13);
    postMessage(result.toString('ascii'));
  } catch (e) {
    postMessage(e);
  }
};
