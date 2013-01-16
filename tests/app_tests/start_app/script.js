var path = require('path');
var os = require('os');
var fs = require('fs');
var cp = require('child_process');
var exec = cp.exec;
var sqawn = cp.sqawn;

var required_file_win = [
   'ffmpegsumo.dll',
   'icudt.dll',
   'libEGL.dll',
   'libGLESv2.dll',
   'nw.exe',
   'nw.pak'
];
var source_file = ['index.html', 'package.json'];

var required_file;
if (os.platform() == 'win32') {
  required_file = required_file_win;
}

var exec_root = path.dirname(process.execPath);

exports.getExecPath = function() {
  if (os.platform() == 'win32') {
    return path.join('tmp', 'nw.exe');
  }
  
  
}

function copyExecFiles() {
  fs.mkdir('tmp');
  for (var i in required_file) {
    var src_file = path.join(exec_root, required_file[i]);
    var dst_file = path.join('tmp', required_file[i]);
    if (!fs.existsSync(dst_file)) {
      fs.createReadStream(src_file).pipe(fs.createWriteStream(dst_file));
    }
  }
  
}

exports.copySourceFiles = function() { 
  if (!fs.existsSync('tmp/index.html')) {
    fs.createReadStream('app_tests/start_app/index.html').pipe(fs.createWriteStream('tmp/index.html'));
    fs.createReadStream('app_tests/start_app/package.json').pipe(fs.createWriteStream('tmp/package.json'));
  }

}

exports.zipSourceFiles = function(callback) {
  if (!fs.existsSync('tmp/app.nw')) {
    exec('python app_tests/start_app/zip.py',
        function() {
      callback();
    }); 
  } else {
    callback();
  }
  
}

exports.makeExecuableFile = function() {
  if (os.platform() == 'win32' && !fs.existsSync('tmp/app.exe')) {  
    cp.execFile(path.join('app_tests', 'start_app', 'pak.bat'));      
  }
}
exports.copyExecFiles = copyExecFiles;