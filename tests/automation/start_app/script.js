var path = require('path');
var os = require('os');
var fsextra = require('fs-extra');
var cp = require('child_process');
var exec = cp.exec;
var sqawn = cp.sqawn;
var global = {};
global.tests_dir = fsextra.realpathSync('.');

var required_file_win_linux = fsextra.readdirSync(path.dirname(process.execPath));
var required_file_win = required_file_win_linux;
var required_file_linux = required_file_win_linux;

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
  if (~exec_root.indexOf("Helper.app"))
    exec_root = path.join(exec_root, '..', '..', '..')
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

function copyExecFiles(done) {
  fsextra.mkdir('tmp-nw', function(err) {
    if(err && err.code !== 'EEXIST') throw err;
    var files_done = 0;
    for (var i in required_file) {
      var src_file = path.join(exec_root, required_file[i]);
      var dst_file = path.join('tmp-nw', required_file[i]);
      fsextra.copySync(src_file, dst_file);
    }
    done();
  });

}

exports.copySourceFiles = function(folder) {
  if (folder == undefined)
	folder = 'start_app';
  fsextra.createReadStream(global.tests_dir + '/' + folder + '/index.html').pipe(
      fsextra.createWriteStream('tmp-nw/index.html'));
  fsextra.createReadStream(global.tests_dir + '/' + folder + '/package.json').pipe(
      fsextra.createWriteStream('tmp-nw/package.json'));

}

exports.zipSourceFiles = function(callback) {
  exec('python '+path.join(global.tests_dir, 'zip.py'));
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
