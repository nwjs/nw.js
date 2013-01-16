var path = require('path');
var os = require('os');
var fs = require('fs-extra');
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

var required_file_linux = [
  'nw',
  'nw.pak',
  'libffmpegsumo.so'
];

var required_file_macox = [
  'node-webkit.app'                         
];

var source_file = ['index.html', 'package.json'];

var exec_root = path.dirname(process.execPath);
var required_file;
if (os.platform() == 'win32') {
  required_file = required_file_win;
}
if (os.platform() == 'linux') {
  required_file = required_file_linux;
}
if (os.platform() == 'darwin') {
  required_file = required_file_macox;
  exec_root = path.normalize(
      path.join(exec_root, '..', '..', '..'));
}



exports.getExecPath = function() {
  if (os.platform() == 'win32') {
    return path.join('tmp-nw', 'nw.exe');
  }
  if (os.platform() == 'linux') {
    return path.join('tmp-nw', 'nw');
  }
  if (os.platform() == 'darwin') {
    return path.join('tmp-nw', 'node-webkit.app', 'Contents', 'MacOS', 'node-webkit');
  }
  
}

function copyExecFiles() {
  fs.mkdir('tmp-nw');
  
  for (var i in required_file) {
    var src_file = path.join(exec_root, required_file[i]);
    var dst_file = path.join('tmp-nw', required_file[i]);
    //fs.copyFileSync(src_file, dst_file);
    fs.copy(src_file, dst_file);
  }
  
}

exports.copySourceFiles = function() { 
  fs.createReadStream('app_tests/start_app/index.html').pipe(fs.createWriteStream('tmp-nw/index.html'));
  fs.createReadStream('app_tests/start_app/package.json').pipe(fs.createWriteStream('tmp-nw/package.json'));
}

exports.zipSourceFiles = function(callback) {
  exec('python app_tests/start_app/zip.py');
  setTimeout(callback, 2000);

}

exports.makeExecuableFile = function() {
  if (os.platform() == 'win32') { 
    cp.exec('copy /b nw.exe+app.nw app.exe', {cwd: './tmp-nw/'});    
  }
  if (os.platform() == 'linux') { 
    cp.exec('cat nw app.nw > app && chmod +x app', {cwd: './tmp-nw/'});    
  }
  
}


exports.copyExecFiles = copyExecFiles;