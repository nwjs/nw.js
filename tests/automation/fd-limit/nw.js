var spawn = require('child_process').spawn;
var net = require('net');

exports.port = 13030;

exports.spawn = function(args, cb) {
    var shutdown = function (err, data) {
        shutdown = function(){};
        server.close(function(){
            if (err) {
                cb(err);
            } else {
                cb(null, JSON.parse(data));
            }
        });
    };

    var server = net.createServer();
    server.on('connection', function(con) {
        con.on('data', function(data) {
            shutdown(null, data);
        });
        con.on('error', function(err){
            shutdown(err);
        });
        con.on('close', function(){
            shutdown(new Error('not-receive'));
        });
    });
    server.listen(exports.port);

    return spawn(process.execPath, args);

};

exports.send = function(data) {
    var client = net.connect(exports.port);
    client.end(JSON.stringify(data));
    require('nw.gui').App.quit();
};
