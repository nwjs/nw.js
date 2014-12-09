exports.xhr = function () {
    global.xhr.onreadystatechange = function() {
        if (global.xhr.readyState === 4 && global.xhr.status === 200) {
        }
    }
    global.xhr.onload = function(e) {
        var uInt8Array = new Uint8Array(this.response); // this.response == uInt8Array.buffer                                                                                     
        // var byte3 = uInt8Array[4]; // byte at offset 4                                                                                                                         
    };

}