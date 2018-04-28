onmessage = (event) => {
    console.log('Trying to connect');

    const http2 = require('http2');
    var connection = http2.connect('https://github.com'); // returns http2session
    connection.on('connect', () => {
        // this never fires, it will fire when used in non-worker context
        console.log('connected');
    });
};