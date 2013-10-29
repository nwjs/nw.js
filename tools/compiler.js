/* (C) 2013 True Interactions - Released under the MIT License.
 * http://www.trueinteractions.com
 * 
 * Overview:
 * This tool configures, builds and writes a complete MacOSX and Windows executable for
 * Node-Webkit and Tint. The input is a set of configuration values, the output is an executable file
 * and .app file fully stamped with meta information and relevant icon (PNG 512x512) provided. 
 * 
 * Why?
 * All files packaged with this method can be read in as embed://File/Name off of the specified source dir.
 * Note this method packages files up as resources in MacOSX/Windows and uses memory mapping to pull resources
 * as needed (e.g., its fast).  In addition it obscures source data with gzip/bit shift operations. Not 100% 
 * secure in any way, but for lamen will present small hurdles.
 *
 * Example:
 *	// cb_error - no longer can continue, passes one parameter;  an error obj or string with the error msg.
 *  // cb_warning - can still continue, but something considerable happened, passes in the warning as a string.
 *  // cb_progress - progress has been made, passes in string with information on whats being done.
 *  // cb_success - The build has finished and was successful.
 *  // cb_start - A build has started, passes in integer for number of steps to build.
 *
 *  var build = new $tint.builder(cb_error,cb_warning,cb_progress,cb_success,cb_start);
 *  build.data.ApplicationId = 123456; 	// An integer representing your application (32 bits). This is used in
 *										// windows to identify your application, cache icons and other meta data.
 *  build.data.DisplayName = 'Your Apps Name';
 *  build.data.BundleName = 'FileNameExec' // Filename of application without extension
 *  build.data.IconMacOSX=build.data.IconWindows = '512x512pngfile.png' // Icon, must be 512x512 rgba (4 channel) png.
 *  build.data.Namespace = 'com.company.product'; // Namespace for identifying your app in MacOSX to assign icons/meta data.
 *  build.data.DisplayVersion = 'M.m.p'; // M=Major Version; m=Minor Version; p=Patch/Build number. Displayed in about screen on OSX
 *  build.data.BuildNumber = '123' // INTEGER ONLY; Identifies version in MacOSX and Windows to tell same app/diff version apart.
 *  build.data.Website = 'www.foo.com' // Website stored in application manifest.
 *  build.data.Copyright = '(c) Copyright 2013' // Copyright stored in application manifest.
 *  build.data.SourceDirectory = '/foo/bar' // Source and (effective root dir) for the package, all the html/js/css you want to include.
 *  build.data.SourceExcludes = 'regex' // Exclude the following (regular expression) from the source directory
 *  build.data.DestinationDirectory = '/out' // The absolute path to where to write the destination files.
 *  build.data.UserAgent = 'User agent info' // Stored in application manifest. 
 *  build.data.Dockless = false; // In MacOSX make the application start without a dock icon by manifest rule.
 *  build.data.SingleInstance = true; // In MacOSX make the application only run as a single instance by manifest rule.
 *
 *  if(build.prepclean())
 *  	build.play();
 *  if(build.prepbuild())
 *  	build.play();
 */

var pa = require('path');
var fs = require('fs');
var os = require('os');
var zlib = require('zlib');
var util = require('util');
var Stream = require('stream');

if(typeof($tint)=='undefined') $tint = {};

$tint.builder = function(onError,onWarning,onProgress,onSuccess,onStart) { 
	return {
		onError:onError,
		onWarning:onWarning,
		onProgress:onProgress,
		onSuccess:onSuccess,
		onStart:onStart,
		blankdata:{
	    	ApplicationId: '', DisplayName: '', BundleName: '', IconMacOSX: '', IconWindows: '', Namespace: '', DisplayVersion: '', BuildNumber: '',
	    	Website: '', Copyright: '', SourceDirectory: '', SourceExcludes: '', DestinationDirectory: '', UserAgent: '', Dockless: false, SingleInstance: true
		},
		data:{
	    	ApplicationId: '', DisplayName: '', BundleName: '', IconMacOSX: '', IconWindows: '', Namespace: '', DisplayVersion: '', BuildNumber: '',
	    	Website: '', Copyright: '', SourceDirectory: '', SourceExcludes: '', DestinationDirectory: '', UserAgent: '', Dockless: false, SingleInstance: true
		},
		tasks:[],
		windowsicon:[],
		macosxicon:[],
		checkdata:function () {
			if (this.data.DisplayName.trim() == "") throw new Error("The display name must contain at least one character.");
			if (this.data.BuildNumber == "" || isNaN(parseInt(this.data.BuildNumber))) throw new Error("The build number must be a whole number.");
			if (this.data.SourceDirectory.trim() == "") throw new Error("A source directory has not been selected.");
			if (this.data.DestinationDirectory.trim() == "") throw new Error("A destination directory has not been selected.");
			if (this.data.BundleName.trim() == "") throw new Error("The bundle name is invalid, bundle name must be a valid file name without an extension or special characters.");
			if ($tint.ndef(this.data.IconMacOSX) || !$tint.file(this.data.IconMacOSX) || this.data.IconMacOSX.indexOf(".png") == -1)
		    	throw new Error("You must select an icon (as a PNG image) to build an application.");
		    if ($tint.ndef(this.data.IconWindows) || !$tint.file(this.data.IconWindows) || this.data.IconWindows.indexOf(".png") == -1)
		    	throw new Error("You must select an icon (as a PNG image) to build an application.");
		},
		config:function() {
			var obj = {};
			// Determine from our process where the resources directory may be, 
			// give a few options to check before giving up.
			var runproc = process.execPath.split(pa.sep);
			if(os.platform()=='darwin' && runproc[runproc.length-1].indexOf('Helper') != -1) obj.rescdir=$tint.path(runproc.slice(0,runproc.length-7).concat('Resources'));
			else if (os.platform()=='darwin' && runproc[runproc.length-1].indexOf('.app') != -1) obj.rescdir=$tint.path(runproc.slice(0,runproc.length-1).concat('Resources'));
			else if (os.platform()!='darwin' && runproc[runproc.length-1].indexOf('.exe') != -1) obj.rescdir=$tint.path(runproc.slice(0,runproc.length-1).concat('Resources'));
			else throw new Error('Cannot determine where the Resources directory may exist.');
			if(!$tint.dir(obj.rescdir)) throw new Error('Cannot find the Resources directory. Expected it at: '+obj.rescdir);
			// Create build configuration
			obj.srcex=this.data.SourceExcludes == '' ? null : this.data.SourceExcludes;
			obj.dstdir=$tint.path([this.data.DestinationDirectory, 'Build']);
			obj.srcdir=$tint.path([this.data.SourceDirectory]);
			obj.pkgmid=$tint.path([obj.dstdir, 'Package']);
			obj.runtime=$tint.path([obj.rescdir, 'Runtime']);
			obj.macapp=$tint.path([this.data.DestinationDirectory, this.data.BundleName + '.app']);
			obj.winapp=$tint.path([this.data.DestinationDirectory, this.data.BundleName + '.exe']);
			var maccontents = $tint.path([obj.macapp,'Contents']);
			var macresources = $tint.path([maccontents,'Resources']);
			var macframeworks = $tint.path([maccontents,'Frameworks']);
			obj.macinfo=$tint.path([maccontents, 'Info.plist']);
			obj.macicon=$tint.path([macresources, 'app.icns']);
			obj.macpkgdst=$tint.path([macresources, 'Package']);
			obj.perms=[	$tint.path([maccontents, 'MacOS', 'Runtime']), $tint.path([macframeworks, 'Runtime Framework.framework', 'Runtime Framework']),
						$tint.path([macframeworks, 'Runtime Framework.framework', 'Libraries', 'ffmpegsumo.so']), 
						$tint.path([macframeworks, 'Runtime Helper.app', 'Contents', 'MacOS', 'Runtime Helper']),
						$tint.path([macframeworks, 'Runtime Helper EH.app', 'Contents', 'MacOS', 'Runtime Helper EH']),
						$tint.path([macframeworks, 'Runtime Helper NP.app', 'Contents', 'MacOS', 'Runtime Helper NP']) ];
			obj.icon=$tint.path([this.data.IconPath]);
			obj.manifest=$tint.path([obj.dstdir,'package.json']);
			obj.wrapper=$tint.path([obj.dstdir,'wrapper.html']);
			// Create a list of what to prepare for packaging
			obj.toprepare=obj.topackage=$tint.getfiles(obj.srcdir)
				// filter out excemptions.
				.filter(function (e) { return !e.match(obj.srcex); })
				// create absolute & relative in/out paths.
				.map(function(e) { return $tint.getpaths(e,obj.dstdir,obj.srcdir); })
				// filter out anything going to the destination directory.
				.filter(function(e) { return !$tint.issubdir(e.absin,this.data.DestinationDirectory); }.bind(this))
			// Create a list for the pre-checks needed to succeed. Files that need to be removed, 
			// Directories that should exist prior to running, files that should exist prior to running.
			obj.prechecks={ 
				remove:[obj.dstdir,obj.macapp,obj.winapp,obj.pkgmid].concat(obj.topackage.map(function(e){return e.absout+'.o';})),
				dirs:[obj.srcdir,obj.rescdir],
				files:obj.topackage.map(function(e){return e.absin;})
			};
			return obj;
		},
		reset:function() { this.tasks=[]; },
		tick:function(e) { if(e)this.onProgress(e); if(this.tasks.length){var task=this.tasks.shift(); setTimeout(function(){try{task.bind(this)()}catch(e){return this.onError(e);}}.bind(this),50);}},
		play:function() { this.onStart(this.tasks.length); this.tick(); },
		stop:function() { this.tasks = [function(e){this.onError('Build was stopped.');}.bind(this)]; },
		running:function() { return this.tasks.length != 0; },
		prepbuild:function () { 
			try {
				// Get the configuration, this has already been validated.
				this.checkdata();
				var conf = this.config();
				var prepfunc = function(b){this.tasks.push(function(){$tint.compress(b.absin,b.absout+'.o',function(){ this.tick("Packaging "+b.relin);}.bind(this),function(e){this.onError(e);}.bind(this));}.bind(this));};
				var packfunc = function(b){this.tasks.push(function(){$tint.appendpkg(b.absout+'.o', b.relname, conf.pkgmid); this.tick("Linking "+b.relname);}.bind(this))};
				// Pre-package, read in data, write out temporary files, perform pre-checks to ensure a safe build.
				conf.prechecks.remove.forEach(function(e){this.tasks.push(function(){$tint.remove(e);this.tick("Validating Build");}.bind(this))}.bind(this));
				conf.prechecks.dirs.forEach(function(e){this.tasks.push(function(){$tint.exists(e,false,"Directory does not exist: %s");this.tick("Validating Build");}.bind(this))}.bind(this));
				conf.prechecks.files.forEach(function(e){this.tasks.push(function(){$tint.exists(e,true,"File does not exist: %s");this.tick("Validating Build");}.bind(this))}.bind(this));
				this.tasks.push(function(){this.macosxicon=$tint.read(this.data.IconMacOSX);this.tick("Reading MacOSX Icon");}.bind(this));
				this.tasks.push(function(){this.pngdata=$tint.read(this.data.IconWindows);this.tick("Reading Windows Icon");}.bind(this));
				this.tasks.push(function(){$tint.parsepng(this.pngdata,function(e){this.onError(e);}.bind(this),function(e){this.windowsiconlrg=e;this.tick("Parsing Icon Data"); }.bind(this));}.bind(this) );
				this.tasks.push(function(){this.windowsicon=$tint.resizeicon(this.windowsiconlrg, 512, 512, 48); this.tick("Resizing Icon");}.bind(this));
				// Compress or 'prepare' the objects to the destination folder.
				conf.toprepare.forEach(prepfunc.bind(this));
				// Package these by appending them to a package location with the stamped magic key/file size.
				conf.topackage.forEach(packfunc.bind(this));
				// Post-package, remove temporary files, stamp windows/macosx icons and manifest information to exec.
				this.tasks=this.tasks.concat([
					function(){ $tint.copy(conf.runtime+'.app',conf.macapp); this.tick("Creating MacOSX Application"); }.bind(this),
					function(){ $tint.copy(conf.runtime+'.exe',conf.winapp); this.tick("Creating Windows Application"); }.bind(this),
					function(){ $tint.copy(conf.pkgmid, $tint.makepath($tint.dotdot(conf.macpkgdst))); this.tick("Finalizing MacOSX"); }.bind(this),
					function(){ $tint.append(conf.winapp, conf.pkgmid); this.tick("Finalizing Windows"); }.bind(this),
					function(){ $tint.write(conf.macinfo, $tint.manifest(this.data)); this.tick("Stamping MacOSX"); }.bind(this),
					function(){ if(os.platform()=='darwin') { conf.perms.forEach(function(e){ fs.chmodSync(e,'755'); }.bind(this)); } this.tick("Fixing permissions"); }.bind(this),
					function(){ $tint.stampmacosx(this.macosxicon, conf.macicon); this.tick("Writing icon for MacOSX"); }.bind(this),
					function(){ $tint.stampwindows(this.windowsicon, conf.winapp); $tint.iconcache(this.onWarning); this.tick("Writing icon for Windows"); }.bind(this),
					function(){ this.onSuccess(); }.bind(this)
				]);
			} catch (e) { this.onError(e); return false; }
			return true;
		},
		prepclean:function() {
			try {
				this.checkdata();
				var conf = this.config();
				var packclean = function(b){this.tasks.push(function(){$tint.remove(b.absout+'.o'); this.tick("Cleaning");}.bind(this))};
				conf.topackage.forEach(packclean.bind(this));
				this.tasks=this.tasks.concat([
					function(){ $tint.remove(conf.macapp); this.tick("Cleaning MacOSX Application"); }.bind(this),
					function(){ $tint.remove(conf.winapp); this.tick("Cleaning Windows Application"); }.bind(this),
					function(){ $tint.remove(conf.pkgmid); this.tick("Cleaning"); }.bind(this)
				]);

			} catch(e) { this.onError(e); return false; }
			return true;
		}
	};
}
$tint.objcopy=function(obj) {
	return JSON.parse(JSON.stringify(obj));
}
$tint.ndef=function(e){ 
	if(typeof(e)=='undefined' || e==null || e=='' || e==0 || e==false) return true; 
	else return false; 
}
$tint.def=function(e) {
	if(typeof(e)!='undefined' && e!=null && e!='' && e!=0 && e!=false) return true;
	else return false;
}
$tint.remove=function(path) {
	if(fs.existsSync(path)) {
		if(fs.statSync(path).isDirectory()) {
			var files = fs.readdirSync(path);
			files.forEach(function(file,index){ $tint.remove(pa.join(path,file)); });
			fs.rmdirSync(path);
		} else fs.unlinkSync(path);
	}
	if(fs.existsSync(path)) throw new Error('File or directory could not be removed: '+path);
	return path;
}
$tint.file=function(f) { return fs.existsSync(f) && fs.statSync(f).isFile(); }
$tint.dir=function(d) { return fs.existsSync(d) && fs.statSync(d).isDirectory(); }
$tint.issubdir=function(issub,isparent) {
	if($tint.isrelative(isparent)) 
		throw new Error('Determining parent and child sub directories when parent is relative is not allowed.');
	else if ($tint.isrelative(issub)) {
		var target = $tint.paths([isparent,issub]);
		return $tint.exists(target);
	} else {
		if( $tint.exists(pa.normalize(issub)) && 
			$tint.exists(pa.normalize(issub)) &&
			pa.normalize(isparent).indexOf(pa.normalize(issub))!=-1) 
		return true;
		else return false;
	}
}
$tint.isrelative=function(qdir) { return (qdir[1]==':'||qdir[0]=='/'||qdir[0]=='\\') ? false : true; }
$tint.read=function(___file) { return fs.readFileSync(___file); }
$tint.dotdot=function(e) { return pa.dirname(e); }
$tint.write=function(__file,_data) {
	$tint.makepath($tint.dotdot(__file));
	fs.writeFileSync(__file,_data);
}
$tint.copy=function(src,dst) {
	var filetodir=function(src,dst) { var paths=src.split(pa.sep); return filetofile(src,pa.join(dst,paths[paths.length-1])); };
	var filetofile=function(src,dst) {
		var bytes=1,buf=new Buffer(64*1024),fdr=fs.openSync(src,'r'),fdw=fs.openSync(dst,'w');
		while(fs.writeSync(fdw,buf,0,fs.readSync(fdr,buf,0,buf.length,null)));
		fs.fchmodSync(fdw,$tint.info(src).fileinfo.mode);
		fs.closeSync(fdr); fs.closeSync(fdw);
	};
	var dirtodir=function(src,dst) { 
		var files=$tint.getfiles(src);
		while(srcfile=files.pop()) { 
			var dstfile=pa.join(dst,srcfile.replace(src,''));
			$tint.makepath($tint.dotdot(dstfile));
			filetofile(srcfile,dstfile);
		}
	};
	if(fs.existsSync(dst) && fs.statSync(dst).isDirectory()) return (fs.statSync(src).isDirectory())?dirtodir(src,dst):filetodir(src,dst);
	else return (fs.statSync(src).isDirectory())?dirtodir(src,dst):filetofile(src,dst);
}
$tint.filesize=function(d) {
	var i = -1;
    var byteUnits = [' KB', ' MB', ' GB', ' TB', ' PB', ' EB', ' ZB', ' YB'];
    do { d = d / 1024; i++; } while (d > 1024);
    return Math.max(d, 0.1).toFixed(1) + byteUnits[i];
}
$tint.append=function(dst,src) {
	$tint.makepath($tint.dotdot(dst));
	var bytes=1,buf=new Buffer(64*1024),fdr=fs.openSync(src,'r'),fdw=fs.openSync(dst,'a+');
	while(fs.writeSync(fdw,buf,0,fs.readSync(fdr,buf,0,buf.length,null)));
	fs.closeSync(fdr); fs.closeSync(fdw);
}
$tint.getfiles = function(dir) {
	var isdir=function(e){return fs.statSync(e).isDirectory();}, isfile=function(e){return !isdir(e);};
	var v=[], f=[dir];
	while(f.length) {
		var target=f.shift();
		var d=fs.readdirSync(target).map(function(e){return pa.join(target,e)});
		f = d.filter(isdir).concat(f);
		v = d.filter(isfile).concat(v);
	}
	return v;
}
$tint.exists=function(path,isfile,errormsg) {
	if($tint.ndef(isfile) && $tint.ndef(errormsg)) return fs.existsSync(path);
	if(!fs.existsSync(path) || fs.statSync(path).isDirectory() && isfile) throw new Error(errormsg.replace('%s',path));
	return path;
}
$tint.makedir=function(e){
	if(!fs.existsSync(e))
		fs.mkdirSync(e);return e;
}
$tint.makepath=function(path) {
	var dr = (path[1]==':') ? path.substring(0,2) : '';
	path = (path[1]==':') ? path.substring(2) : path;
	var paths = path.split(pa.sep), p=pa.sep;
	while(paths.length>1) {
		p=pa.join(p,paths.shift());
		$tint.makedir(dr+p);
	}
	return $tint.makedir(dr+pa.join(p,paths.shift()));
}
$tint.getpaths=function(file,dstdir,srcdir) {
	return {
		absin:$tint.absolute(file,srcdir), 
		absout:$tint.absolute($tint.relative(file, srcdir), dstdir),
		relout:$tint.relative(file,dstdir),
		relin:$tint.relative(file,srcdir),
		relname:$tint.relative(file,srcdir).replace(/\\/g,"/")
	};
}
$tint.path=function(ar) { return pa.normalize(ar.join(pa.sep)); }
$tint.relative2=function(file,base) {
	// FILE AND BASE MUST HAVE SOMETHING IN COMMON, OTHERWISE ITS
	// JUST THE FILE RETURNED.
	if((typeof(base)=='undefined' || base==null) && typeof(file)=='undefined' || file==null) throw new Error('Relative paths expected from no inputs.');
	if(typeof(base)=='undefined' || base==null) return file;
	if(typeof(file)=='undefined' || file==null) return base;

	var file_dr = (file[1]==':') ? file[0]+':' : ''; file=file.replace(file_dr, '');
	var base_dr = (base[1]==':') ? base[0]+':' : ''; base=base.replace(base_dr, '');
	if(file_dr!=base_dr) throw new Error('Cannot make a relative path from different drives.');

	file=file.replace(/\\/g,pa.sep).replace(/\//g,pa.sep);
	base=base.replace(/\\/g,pa.sep).replace(/\//g,pa.sep);

	if(_fs.syncExists(file) && _fs.statStync(file).isFile()) file = $tint.dotdot(file);
	if(_fs.syncExists(base) && _fs.statStync(base).isFile()) file = $tint.dotdot(base);

	if(base[0] != '/') throw new Error('Asked for a relative path where the base isnt absolute');
	if(file[0] != '/') throw new Error('Asked for a relative path where the file path isnt absolute');

	return file_dr+pa.relative(base,file);
}
$tint.relative=function(a,b) {
	if(typeof(b)=='undefined' || b==null) return a;
	a=a.replace(/\\/g,pa.sep).replace(/\//g,pa.sep);
	b=b.replace(/\\/g,pa.sep).replace(/\//g,pa.sep);
	if(a[0]==pa.sep || (a[1]==':' && b[1]==':')) {
		a=a.replace(b,'');
		if(a[0]==pa.sep) a=a.substring(1);
	}
	return pa.normalize(a);
}
$tint.absolute=function(a,b) {
	a=a.replace(/\\/g,pa.sep).replace(/\//g,pa.sep);
	b=b.replace(/\\/g,pa.sep).replace(/\//g,pa.sep);
	if(a[0]!=pa.sep&&a[1]!=':') {
		a=pa.normalize(pa.join(b,a));
	}
	return a;
}
$tint.info=function(_file,_base) { 
	return {
		fileinfo:fs.statSync(_file),
		full:_file,
		relative:$tint.relative(_file,_base)
	}; 
}
$tint.iconcache = function(warning) {
	if (os.platform() != 'darwin') {
		$tint.execute('C:\\windows\\system32\\ie4uinit4.exe',['-ClearIconCache'], false, false, function(e){},
			function(e){ if(warning) warning('Unable to refresh icon cache.\n\t'+e.toString()); },function(e){});
		$tint.execute('C:\\windows\\system32\\cmd.exe',['/e:on','/c','DEL','%localappdata%\\IconCache.db','/A'],false,false,function(e){},
			function(e){ if(warning) warning('Unable to delete icon cache.\n\t'+e.toString()); },function(e){});
	}
}
$tint.compress = function(src,dst,succ,err) {
	$tint.makepath($tint.dotdot(dst));
	if(fs.existsSync(dst)) throw new Error('Cannot compression file, it already exists ['+dst+']');
	var gzip=zlib.createGzip(),inp=fs.createReadStream(src),out=fs.createWriteStream(dst);
	inp.on('end',function(e){ succ(); }.bind(this)).on('error',function(e){ err(e); }.bind(this));
	inp.pipe(gzip).pipe(out);
}
$tint.appendpkg=function(file__,name__,pkgfile__) {
	var keybf = new Buffer('\x20\x01\x77\x55\x66\x31'+name__+'\x20\x01\x77\x55\x66\x31');
	var sizebf = new Buffer(8);
	var size = $tint.info(file__).fileinfo.size;
	sizebf.writeUInt32LE(size,0);
	sizebf.writeUInt32LE(0,4);
	fs.appendFileSync(pkgfile__,Buffer.concat([keybf,sizebf]));
	$tint.append(pkgfile__,file__);
}
$tint.dirdiff = function(srcdir,dstdir,regfilt) {
	if(regfilt=='') regfilt=null;
	if(srcdir[srcdir.length-1]!=pa.sep)srcdir+=pa.sep;
	if(dstdir[dstdir.length-1]!=pa.sep)dstdir+=pa.sep;
	var compare = function(sfiles,dfiles,sbase,dbase) {
		var srconly = sfiles.filter(function(q){return !dfiles.some(function(w){return w==q;});});
		var srcnew = sfiles.forEach(function(q){return dfiles.some(function(w){ 
		 	return w==q && fs.statSync(pa.join(dbase,w)).ctime.getTime() < fs.statSync(pa.join(sbase,q)).ctime.getTime();
		});});
		if(!srconly) srconly=[]; if(!srcnew) srcnew=[];
		return srconly.concat(srcnew);
	};
	var srcfiles=$tint.getfiles(srcdir).filter(function(e){ return !e.match(regfilt); });
	var dstfiles=$tint.getfiles(dstdir).filter(function(e){ return !e.match(regfilt); });
	if(!srcfiles.length) return false;
	return compare(srcfiles,dstfiles,srcdir,dstdir);
}
$tint.writepkg = function(files,base,pkgfile) {
	$tint.remove(pkgfile);
	while(file=files.shift())
		$tint.appendpkg($tint.absolute(file,base)+'.o',$tint.relative(file,base),pkgfile);
}
$tint.stampwindows = function(imgdata, dst) {
	// Stamp Windows Icon
	var dstBuffer = imgdata;
	var fd = _fs.openSync(dst,'r+');
	var w = new WindowsExeFile(fd);
	w.WindowsExeRead();
	var pos = w.Resources.Entries[1].Directory.Entries[0].Directory.Entries[0].Data.Icon.getDataPosition();
	var buf = new Buffer(48*48*4);
	for(var i=0; i < dstBuffer.length;i+=4) {
		var row = 48-Math.floor(i/(4*48)), col = i%(48*4), index=(row-1)*(48*4)+col;
		r = dstBuffer[index];
		g = dstBuffer[index+1];
		b = dstBuffer[index+2];
		a = dstBuffer[index+3];
		buf.writeUInt8(b,i);
		buf.writeUInt8(g,i+1);
		buf.writeUInt8(r,i+2);
		buf.writeUInt8(a,i+3);
	}
	_fs.writeSync(fd, buf, 0, buf.length, pos);
	_fs.closeSync(fd);
}
$tint.execute = function(exec,args,ischild,isapp,output,error,exit) {
	//var execd = (os.platform()=='darwin' && isapp) ? '/usr/bin/open' : exec;
	//var argsd = (os.platform()=='darwin' && isapp) ? ['-W',exec].concat(args) : args;
	var child = require('child_process').spawn(exec,args,{detached:ischild});
		child.stderr.on('data',error);
		child.stdout.on('data',output);
		child.on('exit',exit);
		child.on('error',error);
	if(!ischild) child.unref();
}
$tint.mergeobjs = function(obj1,obj2) {
    var obj3 = {};
    for (var attrname in obj1) { obj3[attrname] = obj1[attrname]; }
    for (var attrname in obj2) { obj3[attrname] = obj2[attrname]; }
    return obj3;
}
$tint.stampmacosx = function(imgdata, dst) {
	var buffer = new Buffer(16);
	buffer.writeInt8(105,0);
	buffer.writeInt8(99,1);
	buffer.writeInt8(110,2);
	buffer.writeInt8(115,3);
	buffer.writeInt32BE(8+8+imgdata.length,4);
	buffer.writeInt8(105,8);
	buffer.writeInt8(99,9);
	buffer.writeInt8(48,10);
	buffer.writeInt8(57,11);
	buffer.writeInt32BE(8+imgdata.length,12);
	$tint.write(dst,Buffer.concat([buffer,new Buffer(imgdata)]));
}
$tint.parsepng = function(imgdata,errf,succf){ 
	var pngcodec = new PNG({filetype:4});
	pngcodec.on('metadata',function(meta) { 
		if(meta.width!=512 || meta.height!=512 || !meta.alpha) { 
			errf('Icon must be 512 by 512 pixels with an alpha channel.'); 
			throw new Error('PNGERR');
		} 
	});
	pngcodec.parse(imgdata, function(err,data) { 
		if(err) return errf('The specified icon could not be parsed: '+err); 
		succf(data.data);
	});
}
$tint.resizeicon = function(imgd_, h_, w_, dstw_) {
	var resizefunc = function(simg_, sw, sh, dw, dh, lobes) {
		var obj = {
			src:simg_,
			dst:{data:new Uint8Array(dw*dh*4)},
			lanczos:function(x) {
				if (x > lobes) return 0;
				x *= Math.PI;
				if (Math.abs(x) < 1e-16) return 1;
				var xx = x / lobes;
				return Math.sin(x) * Math.sin(xx) / x / xx;
			},
			ratio:sw / dw,
			rcp_ratio:2 / (sw / dw),
			range2:Math.ceil((sw / dw) * lobes / 2),
			cacheLanc:{},
			center:{},
			icenter:{},
			process:function(self, u) {
				self.center.x = (u + 0.5) * self.ratio;
				self.icenter.x = Math.floor(self.center.x);
				for (var v = 0; v < dh; v++) {
					self.center.y = (v + 0.5) * self.ratio;
					self.icenter.y = Math.floor(self.center.y);
					var a=0, r=0, g=0, b=0, z=0;
					for (var i = self.icenter.x - self.range2; i <= self.icenter.x + self.range2; i++) {
						if (i < 0 || i >= sw) continue;
						var f_x = Math.floor(1000 * Math.abs(i - self.center.x));
						if (!self.cacheLanc[f_x]) self.cacheLanc[f_x] = {};
						for (var j = self.icenter.y - self.range2; j <= self.icenter.y + self.range2; j++) {
							if (j < 0 || j >= sh) continue;
							var f_y = Math.floor(1000 * Math.abs(j - self.center.y));
							if (self.cacheLanc[f_x][f_y] == undefined) self.cacheLanc[f_x][f_y] = self.lanczos(Math.sqrt(Math.pow(f_x * self.rcp_ratio, 2) + Math.pow(f_y * self.rcp_ratio, 2)) / 1000);
							z += (self.cacheLanc[f_x][f_y] < 0) ? 0 : self.cacheLanc[f_x][f_y];
							r += (self.cacheLanc[f_x][f_y] < 0) ? 0 : self.cacheLanc[f_x][f_y] * self.src.data[(j * sw + i) * 4];
							g += (self.cacheLanc[f_x][f_y] < 0) ? 0 : self.cacheLanc[f_x][f_y] * self.src.data[(j * sw + i) * 4 + 1];
							b += (self.cacheLanc[f_x][f_y] < 0) ? 0 : self.cacheLanc[f_x][f_y] * self.src.data[(j * sw + i) * 4 + 2];
							a += (self.cacheLanc[f_x][f_y] < 0) ? 0 : self.cacheLanc[f_x][f_y] * self.src.data[(j * sw + i) * 4 + 3];
						}
					}
					self.dst.data[(v * dw + u) * 4] = r / z;
					self.dst.data[(v * dw + u) * 4 + 1] = g / z;
					self.dst.data[(v * dw + u) * 4 + 2] = b / z;
					self.dst.data[(v * dw + u) * 4 + 3] = a / z;
				}
				if (++u < dw) return self.process(self, u);
				else return self.dst.data;
			}
		};
		return obj.process(obj,0);
	};
	var dsth_ = Math.round(h_*dstw_/w_);
	return resizefunc({data:imgd_},w_,h_,dstw_,dsth_,3);
}
$tint.manifest = function (data) {
    return '<?xml version="1.0" encoding="UTF-8"?>\n' +
	'<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">\n' +
	'<plist version="1.0">\n' +
	'<dict>\n' +
	'\t<key>CFBundleDevelopmentRegion</key>\n' +
	'\t<string>en</string>\n' +
	'\t<key>CFBundleDisplayName</key>\n' +
	'\t<string>' + data.BundleName + '</string>\n' +
	'\t<key>CFBundleDocumentTypes</key>\n' +
	'\t<array>\n' +
	'\t\t<dict>\n' +
	'\t\t\t<key>CFBundleTypeIconFile</key>\n' +
	'\t\t\t<string>app.icns</string>\n' +
	'\t\t\t<key>CFBundleTypeName</key>\n' +
	'\t\t\t<string>' + data.DisplayName + '</string>\n' +
	'\t\t\t<key>CFBundleTypeRole</key>\n' +
	'\t\t\t<string>Viewer</string>\n' +
	'\t\t\t<key>LSHandlerRank</key>\n' +
	'\t\t\t<string>Owner</string>\n' +
	'\t\t\t<key>LSItemContentTypes</key>\n' +
	'\t\t\t<array>\n' +
	'\t\t\t\t<string>' + data.Namespace + '</string>\n' +
	'\t\t\t</array>\n' +
	'\t\t</dict>\n' +
	'\t\t<dict>\n' +
	'\t\t\t<key>CFBundleTypeName</key>\n' +
	'\t\t\t<string>Folder</string>\n' +
	'\t\t\t<key>CFBundleTypeOSTypes</key>\n' +
	'\t\t\t<array>\n' +
	'\t\t\t\t<string>fold</string>\n' +
	'\t\t\t</array>\n' +
	'\t\t\t<key>CFBundleTypeRole</key>\n' +
	'\t\t\t<string>Viewer</string>\n' +
	'\t\t\t<key>LSHandlerRank</key>\n' +
	'\t\t\t<string>None</string>\n' +
	'\t\t</dict>\n' +
	'\t</array>\n' +
	'\t<key>CFBundleExecutable</key>\n' +
	'\t<string>Runtime</string>\n' +
	'\t<key>CFBundleIconFile</key>\n' +
	'\t<string>app.icns</string>\n' +
	'\t<key>CFBundleIdentifier</key>\n' +
	'\t<string>' + data.Namespace + '</string>\n' +
	'\t<key>CFBundleInfoDictionaryVersion</key>\n' +
	'\t<string>6.0</string>\n' +
	'\t<key>CFBundleName</key>\n' +
	'\t<string>' + data.BundleName + '</string>\n' +
	'\t<key>CFBundlePackageType</key>\n' +
	'\t<string>APPL</string>\n' +
	'\t<key>CFBundleShortVersionString</key>\n' +
	'\t<string>' + data.DisplayVersion + '</string>\n' +
	'\t<key>CFBundleVersion</key>\n' +
	'\t<string>' + data.BuildNumber + '</string>\n' +
	'\t<key>LSUIElement</key>\n' +
	'\t<string>' + ((data.Dockless==true) ? '1' : '0') + '</string>\n' +
	'\t<key>LSMultipleInstancesProhibited</key>\n' +
	((data.SingleInstance==true) ? '\t<true/>\n' : '\t<false/>\n') +
	'\t<key>LSFileQuarantineEnabled</key>\n' +
	'\t<true/>\n' +
	'\t<key>LSMinimumSystemVersion</key>\n' +
	'\t<string>10.6.0</string>\n' +
	'\t<key>NSPrincipalClass</key>\n' +
	'\t<string>NSApplication</string>\n' +
	'\t<key>NSSupportsAutomaticGraphicsSwitching</key>\n' +
	'\t<true/>\n' +
	'\t<key>SCMRevision</key>\n' +
	'\t<string>186115</string>\n' +
	'\t<key>UTExportedTypeDeclarations</key>\n' +
	'\t<array>\n' +
	'\t\t<dict>\n' +
	'\t\t\t<key>UTTypeConformsTo</key>\n' +
	'\t\t\t<array>\n' +
	'\t\t\t\t<string>none</string>\n' +
	'\t\t\t</array>\n' +
	'\t\t\t<key>UTTypeDescription</key>\n' +
	'\t\t\t<string>' + data.DisplayName + '</string>\n' +
	'\t\t\t<key>UTTypeIconFile</key>\n' +
	'\t\t\t<string>app.icns</string>\n' +
	'\t\t\t<key>UTTypeIdentifier</key>\n' +
	'\t\t\t<string>' + data.Namespace + '</string>\n' +
	'\t\t\t<key>UTTypeReferenceURL</key>\n' +
	'\t\t\t<string>' + data.Website + '</string>\n' +
	'\t\t\t<key>UTTypeTagSpecification</key>\n' +
	'\t\t\t<dict>\n' +
	'\t\t\t\t<key>com.apple.ostype</key>\n' +
	'\t\t\t\t<string>ti</string>\n' +
	'\t\t\t\t<key>public.filename-extension</key>\n' +
	'\t\t\t\t<array>\n' +
	'\t\t\t\t\t<string></string>\n' +
	'\t\t\t\t</array>\n' +
	'\t\t\t\t<key>public.mime-type</key>\n' +
	'\t\t\t\t<string></string>\n' +
	'\t\t\t</dict>\n' +
	'\t\t</dict>\n' +
	'\t</array>\n' +
	'</dict>\n' +
	'</plist>\n';
}
/** Constants **/
var WindowsConst={};
WindowsConst.RESOURCE_ENTRY_TYPES = [
	WindowsConst.RT_UNKNOWN = {value:0,name:'RT_UNKNOWN'}, WindowsConst.RT_CURSOR = {value:1,name:'RT_CURSOR'}, 
	WindowsConst.RT_BITMAP = {value:2,name:'RT_BITMAP'}, WindowsConst.RT_ICON = {value:3,name:'RT_ICON'}, 
	WindowsConst.RT_MENU = {value:4,name:'RT_MENU'}, WindowsConst.RT_DIALOG = {value:5,name:'RT_DIALOG'}, 
	WindowsConst.RT_STRING = {value:6,name:'RT_STRING'}, WindowsConst.RT_FONTDIR = {value:7,name:'RT_FONTDIR'},
	WindowsConst.RT_FONT = {value:8,name:'RT_FONT'}, WindowsConst.RT_ACCELERATOR=  {value:9,name:'RT_ACCELERATOR'},
	WindowsConst.RT_RCDATA = {value:10,position:-1,name:'RT_RCDATA'}, WindowsConst.RT_MESSAGETABLE = {value:11,name:'RT_MESSAGETABLE'},
	WindowsConst.RT_GROUP_CURSOR = {value:12,name:'RT_GROUP_CURSOR'}, WindowsConst.RT_UNKNOWN = {value:0,name:'RT_UNKNOWN'}, 
	WindowsConst.RT_GROUP_ICON = {value:14,name:'RT_GROUP_ICON'}, WindowsConst.RT_UNKNOWN = {value:0,name:'RT_UNKNOWN'}, 
	WindowsConst.RT_VERSION = {value:16,name:'RT_VERSION'}, WindowsConst.RT_DLGINCLUDE = {value:17,name:'RT_DLGINCLUDE'}, 
	WindowsConst.RT_UNKNOWN= {value:0,name:'RT_UNKNOWN'}, WindowsConst.RT_PLUGPLAY = {value:19,name:'RT_PLUGPLAY'},
	WindowsConst.RT_VXD = {value:20,name:'RT_VXD'}, WindowsConst.RT_ANICURSOR = {value:21,name:'RT_ANICURSOR'}, 
	WindowsConst.RT_ANIICON = {value:22,name:'RT_ANIICON'},WindowsConst.RT_HTML= {value:23,name:'RT_HTML'}, 
	WindowsConst.RT_MANIFEST = {value:24,name:'RT_MANIFEST'}
];
WindowsConst.IMAGE_DOS_SIGNATURE 				= {value:23117, name:'MSDOS'};
WindowsConst.IMAGE_OS2_SIGNATURE 				= {value:17742, name:'OS2'};
WindowsConst.IMAGE_OS2_SIGNATURE_LE 				= {value:17740, name:'OS2 LE'};
WindowsConst.IMAGE_NT_SIGNATURE 					= {value:17744, name:'NT'};
WindowsConst.IMAGE_FILE_MACHINE_I386				= {value:332, name:'i386'};
WindowsConst.IMAGE_FILE_MACHINE_IA64				= {value:512, name:'ia64'};
WindowsConst.IMAGE_FILE_MACHINE_AMD64			= {value:34404, name:'amd64'};
WindowsConst.IMAGE_DIRECTORY_ENTRY_EXPORT 		= 0;		// Export Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_IMPORT 		= 1;		// Import Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_RESOURCE 		= 2;		// Resource Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_EXCEPTION 	= 3;		// Exception Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_SECURITY 		= 4;		// Security Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_BASERELOC 	= 5;		// Base Relocation Table
WindowsConst.IMAGE_DIRECTORY_ENTRY_DEBUG 		= 6;		// Debug Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_COPYRIGHT 	= 7;		// Description String
WindowsConst.IMAGE_DIRECTORY_ENTRY_GLOBALPTR 	= 8;		// Machine Value (MIPS GP)
WindowsConst.IMAGE_DIRECTORY_ENTRY_TLS 			= 9;		// TLS Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG 	= 10;		// Load Configuration Directory
WindowsConst.IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	= 11;
WindowsConst.IMAGE_DIRECTORY_ENTRY_IAT			= 12;
WindowsConst.IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	= 13;
WindowsConst.IMAGE_DIRECTORY_ENTRY_CLR_RUNTIME	= 14;
WindowsConst.IMAGE_DIRECTORY_ENTRY_RESERVED		= 15;
WindowsConst.IMAGE_SIZEOF_SHORT_NAME 			= 8;
WindowsConst.IMAGE_NUMBEROF_DIRECTORY_ENTRIES 	= 16;
WindowsConst.SIZE_OF_NT_SIGNATURE 				= 4;
WindowsConst.WINDOWS_VERSIONS = [
	{Name:'Windows 8', MajorOperatingSystemVersion:6, MinorOperatingSystemVersion:2 },
	{Name:'Windows 7', MajorOperatingSystemVersion:6, MinorOperatingSystemVersion:1 },
	{Name:'Windows Vista', MajorOperatingSystemVersion:6, MinorOperatingSystemVersion:0 },
	{Name:'Windows XP 64-Bit Edition', MajorOperatingSystemVersion:5, MinorOperatingSystemVersion:2 },
	{Name:'Windows XP', MajorOperatingSystemVersion:5, MinorOperatingSystemVersion:1 },
	{Name:'Windows 2000', MajorOperatingSystemVersion:5, MinorOperatingSystemVersion:0 }
];
/** Helper Functions **/
WindowsConst.LOWORD = function(e) { return (e.value & 0x0000ffff); }
WindowsConst.HIGHBIT = function(e) { return (0x80000000 & e) != 0; }
WindowsConst.STRIPHIGHBIT = function(e) { return ((~0x80000000) & e); }
WindowsConst.GETOFFSETBYADDRESS = function(address, winObj) {
	for(var i=0; i < winObj.SectionHeaders.length; i++)
	{
		var VABegin = winObj.SectionHeaders[i].VirtualAddress;
		var VAEnd = winObj.SectionHeaders[i].SizeOfRawData + VABegin;
		if( VABegin <= address && VAEnd > address )
			return address - winObj.SectionHeaders[i].VirtualOffset;
	}
	return 0;	
}
WindowsConst.GETOFFSETBYDIRECTORY = function(directory, winObj) {
	return WindowsConst.GETOFFSETBYADDRESS(winObj.OptionalHeader.DataDirectory[directory].VirtualAddress, winObj);
}
WindowsConst.READ = function(size, wef) {
	var buf = new Buffer(size);
	_fs.readSync(wef.FileDescriptor, buf, 0, size, wef.Position);
	wef.Increment(size);
	return buf;
}
/** Objects and Structures **/
var WindowsExeFile = function(fd)
{
	this.name = 'WindowsPEFile';
	this.FileDescriptor = fd;
	this.Position = 0;
}
WindowsExeFile.prototype.BOOL = function() { return WindowsConst.READ(4, this).readUInt32LE(0); }
WindowsExeFile.prototype.BOOLEAN = function() { return WindowsConst.READ(1, this).readUInt8(0); }
WindowsExeFile.prototype.BYTE = function() { return WindowsConst.READ(1, this).readUInt8(0); };
WindowsExeFile.prototype.UCHAR = function() { return WindowsConst.READ(1, this).toString('ascii'); }
WindowsExeFile.prototype.USHORT = function() { return WindowsConst.READ(2, this).readUInt16LE(0); }
WindowsExeFile.prototype.LONG = function() { return WindowsConst.READ(4, this).readInt32LE(0); }
WindowsExeFile.prototype.ULONG = function() { return WindowsConst.READ(4, this).readUInt32LE(0); }
WindowsExeFile.prototype.WCHAR =  function() { return WindowsConst.READ(2, this).toString('utf8'); }
WindowsExeFile.prototype.DWORD = function() { return WindowsConst.READ(4, this).readUInt32LE(0); }
WindowsExeFile.prototype.WORD = function() { return WindowsConst.READ(2, this).readUInt16LE(0); }
WindowsExeFile.prototype.Increment = function(e) { return (this.Position = this.Position + e); }
WindowsExeFile.prototype.ResourceDataIconRead = function() {
 	var obj = {};
	obj.biSize 			= this.DWORD();
	obj.biWidth			= this.LONG();
	obj.biHeight		= this.LONG();
	obj.biPlanes		= this.WORD();
	obj.biBitCount		= this.WORD();
	obj.biCompression	= this.DWORD();
	obj.biSizeImage		= this.DWORD();
	obj.biXPelsPerMeter	= this.LONG();
	obj.biYPelsPerMeter	= this.LONG();
	obj.biClrUsed		= this.DWORD();
	obj.biClrImportant	= this.DWORD();
	obj.Position		= this.Position;
	obj.getDataPosition = function() { return this.Position; };
	obj.getDataSize 	= function() { return (this.biSizeImage == 0) ? obj.biWidth*(obj.biHeight/2)*(obj.biBitCount/8) : this.biSizeImage; };
	return obj;
};
WindowsExeFile.prototype.ResourceDataGroupIconRead = function() {
	var obj = {};
	obj.wReserved 			= this.WORD();		// Currently zero 
	obj.wType				= this.WORD();		// 1 for icons 
	obj.wCount				= this.WORD();		// Number of components 
	obj.Entries				= new Array();
	for(var i=0; i < obj.wCount; i++) {
		var sObj = {};
		sObj.bWidth 		= this.BYTE();
		sObj.bHeight 		= this.BYTE();
		sObj.bColorCount 	= this.BYTE();
		sObj.bReserved 		= this.BYTE();
		sObj.wPlanes 		= this.WORD();
		sObj.wBitCount 		= this.WORD();
		sObj.lBytesInRes 	= this.DWORD();
		sObj.wNameOrdinal 	= this.WORD();
		obj.Entries.push(sObj);
	}
	return obj;
}
WindowsExeFile.prototype.ResourceDataRead = function(p) {
	var obj = {}
	obj.parent			= p;
	obj.OffsetToData 	= this.ULONG();
	obj.Size			= this.ULONG();
	obj.CodePage		= this.ULONG();
	obj.Reserved		= this.ULONG();
	obj.PhysicalAddress	= WindowsConst.GETOFFSETBYADDRESS(obj.OffsetToData, this);
	try {
		/* Crawl up the chain to get our type and language */
		var index = obj.parent.parent.parent.parent.parent.Name;
		if(index > WindowsConst.RESOURCE_ENTRY_TYPES.length) obj.ResourceType = WindowsConst.RT_UNKNOWN;
		else obj.ResourceType = WindowsConst.RESOURCE_ENTRY_TYPES[index];
		
		var SavePosition = this.Position;
		this.Position = obj.PhysicalAddress;

		switch(obj.ResourceType.value) {
			case WindowsConst.RT_ICON.value:
				obj.Icon = this.ResourceDataIconRead();
				break;
			case WindowsConst.RT_GROUP_ICON.value:
				obj.GroupIcon = this.ResourceDataGroupIconRead();
				break;
		}
		
		this.Position = SavePosition;
	} catch(e) {
		obj.ResourceType = WindowsConst.RT_UNKNOWN;
		obj.ErrorOccured = 'Cannot read resources, an unknown type was encountered.';
	}
	return obj;
}
WindowsExeFile.prototype.ResourceStringRead = function(p) {
	var obj			= {};
	obj.Length 		= this.ULONG();
	obj.NameString	= this.WCHAR();
	return obj;
}
WindowsExeFile.prototype.ResourceEntryRead = function(p) {
	var obj = {};
	obj.parent			= p;
	obj.Name			= this.ULONG();
	obj.OffsetToData	= this.ULONG();

	var SavePosition	= this.Position;
	this.Position 		= this.ResourcePosition + WindowsConst.STRIPHIGHBIT(obj.OffsetToData);
	
	if(WindowsConst.HIGHBIT(obj.OffsetToData)) obj.Directory = this.ResourceDirectoryRead(obj);
	else obj.Data = this.ResourceDataRead(obj);
	
	this.Position 		= SavePosition;
	
	return obj;
}
WindowsExeFile.prototype.ResourceDirectoryRead = function(p) {
	var obj = {};
	obj.parent					= p;
	obj.Characteristics			= this.ULONG();
	obj.TimeDateStamp 			= new Date(this.ULONG()*1000);
	obj.MajorVersion			= this.USHORT();
	obj.MinorVersion			= this.USHORT();
	obj.NumberOfNamedEntries 	= this.USHORT();
	obj.NumberOfIdEntries		= this.USHORT();
	obj.Entries					= new Array();
	
	var SavePosition			= this.Position;

	for(var i=0; i < obj.NumberOfNamedEntries + obj.NumberOfIdEntries; i++)
		obj.Entries.push( this.ResourceEntryRead(obj) );

	this.Position = SavePosition;
	
	return obj;
}
WindowsExeFile.prototype.SectionHeaderRead = function() {
	var obj = {};
	obj.Name = ''.concat(
		this.UCHAR(), this.UCHAR(), this.UCHAR(), this.UCHAR(),
		this.UCHAR(), this.UCHAR(), this.UCHAR(), this.UCHAR()
	);
	obj.Misc = this.ULONG();
	obj.PhysicalAddress = obj.Misc;
	obj.VirtualSize = obj.Misc;
	obj.VirtualAddress = this.ULONG();
	obj.SizeOfRawData = this.ULONG();
	obj.PointerToRawData = this.ULONG();
	obj.PointerToRelocations = this.ULONG();
	obj.PointerToLinenumbers = this.ULONG();
	obj.NumberOfRelocations = this.USHORT();
	obj.NumberOfLinenumbers = this.USHORT();
	obj.Characteristics = this.ULONG();	
	obj.VirtualOffset = obj.VirtualAddress - obj.PointerToRawData;
	return obj;
}
WindowsExeFile.prototype.SectionHeadersRead = function() {
	var SectionHeaders = new Array();
	for(var i=0; i < this.FileHeader.NumberOfSections; i++)
		SectionHeaders.push( this.SectionHeaderRead() );
	return SectionHeaders;
}
WindowsExeFile.prototype.DataDirectoryRead = function() {
	var obj = {};
	obj.VirtualAddress = this.ULONG();
	obj.Size = this.ULONG();
	return obj;
}
WindowsExeFile.prototype.OptionalHeaderRead = function() {
	var obj = {};
	obj.Magic = this.USHORT();
    obj.MajorLinkerVersion = this.UCHAR();
    obj.MinorLinkerVersion = this.UCHAR();
    obj.SizeOfCode = this.ULONG();
    obj.SizeOfInitializedData = this.ULONG();
    obj.SizeOfUninitializedData = this.ULONG();
    obj.AddressOfEntryPoint = this.ULONG();
    obj.BaseOfCode = this.ULONG();
    obj.BaseOfData = this.ULONG();
    obj.ImageBase = this.ULONG();
    obj.SectionAlignment = this.ULONG();
    obj.FileAlignment = this.ULONG();
    obj.MajorOperatingSystemVersion = this.USHORT();
    obj.MinorOperatingSystemVersion = this.USHORT();
    obj.MajorImageVersion = this.USHORT();
    obj.MinorImageVersion = this.USHORT();
    obj.MajorSubsystemVersion = this.USHORT();
    obj.MinorSubsystemVersion = this.USHORT();
    obj.Reserved1 = this.ULONG();
    obj.SizeOfImage = this.ULONG();
    obj.SizeOfHeaders = this.ULONG();
    obj.CheckSum = this.ULONG();
    obj.Subsystem = this.USHORT();
    obj.DllCharacteristics = this.USHORT();
	obj.SizeOfStackReserve = this.ULONG();
    obj.SizeOfStackCommit = this.ULONG();
    obj.SizeOfHeapReserve = this.ULONG();
    obj.SizeOfHeapCommit = this.ULONG();
    obj.LoaderFlags = this.ULONG();
    obj.NumberOfRvaAndSizes = this.ULONG();
    obj.DataDirectory = new Array();
	
	for(var i=0; i < WindowsConst.WINDOWS_VERSIONS.length; i++)
		if(WindowsConst.WINDOWS_VERSIONS[i].MajorOperatingSystemVersion == obj.MajorOperatingSystemVersion &&
			WindowsConst.WINDOWS_VERSIONS[i].MinorOperatingSystemVersion == obj.MinorOperatingSystemVersion )
			obj.WindowsVersion = WindowsConst.WINDOWS_VERSIONS[i];

	for(var i=0; i < WindowsConst.IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
		obj.DataDirectory.push(this.DataDirectoryRead());

	return obj;
}
WindowsExeFile.prototype.FileHeaderRead = function() {
	var obj = {}
	obj.Machine = this.USHORT();
	obj.Machine = (WindowsConst.IMAGE_FILE_MACHINE_I386.value == obj.Machine) ? WindowsConst.IMAGE_FILE_MACHINE_I386 : 
		( (WindowsConst.IMAGE_FILE_MACHINE_IA64.value == obj.Machine) ? WindowsConst.IMAGE_FILE_MACHINE_IA64 : WindowsConst.IMAGE_FILE_MACHINE_AMD64 );
	obj.NumberOfSections = this.USHORT();
	obj.TimeDateStamp = new Date(this.ULONG()*1000);
	obj.PointerToSymbolTable = this.ULONG();
	obj.NumberOfSymbols = this.ULONG();
	obj.SizeOfOptionalHeader = this.USHORT();
	obj.Characteristics = this.USHORT();
	return obj;
}
WindowsExeFile.prototype.FileTypeRead = function() {
	var ImageFileTypeWord = this.DWORD();
	
	// Determine the type of PE executable
	if(WindowsConst.LOWORD(ImageFileTypeWord) == WindowsConst.IMAGE_OS2_SIGNATURE.value) return WindowsConst.IMAGE_OS2_SIGNATURE;
	else if (WindowsConst.LOWORD(ImageFileTypeWord) == WindowsConst.IMAGE_OS2_SIGNATURE_LE.value) return WindowsConst.IMAGE_OS2_SIGNATURE_LE;
	else if (ImageFileTypeWord == WindowsConst.IMAGE_NT_SIGNATURE.value) return WindowsConst.IMAGE_NT_SIGNATURE;
	else if (ImageFileTypeWord == WindowsConst.IMAGE_DOS_SIGNATURE.value) return WindowsConst.IMAGE_DOS_SIGNATURE;
	else return {value:ImageFileTypeWord, name:'UNKNOWN'};
}
WindowsExeFile.prototype.DosHeaderRead = function() {
	var obj = {}
	obj.e_magic = this.USHORT();	// Magic number
	if(obj.e_magic != WindowsConst.IMAGE_DOS_SIGNATURE.value) 
		throw new {name:'NotWindowsPEFile', message:'This does not appear to be a valid Windows PE file.'};
	
	obj.e_cblp = this.USHORT();		// Bytes on last page of file
	obj.e_cp = this.USHORT();		// Pages in file
	obj.e_crlc = this.USHORT();		// Relocations
	obj.e_cparhdr = this.USHORT();	// Size of header in paragraphs
	obj.e_minalloc = this.USHORT();	// Minimum extra paragraphs needed
	obj.e_maxalloc = this.USHORT();	// Maximum extra paragraphs needed
	obj.e_ss = this.USHORT();		// Initial (relative) SS value
	obj.e_sp = this.USHORT();		// Initial SP value
	obj.e_csum = this.USHORT();		// Checksum
	obj.e_ip = this.USHORT();		// Initial IP value
	obj.e_cs = this.USHORT();		// Initial (relative) CS value
	obj.e_lfarlc = this.USHORT();	// File address of relocation table
	obj.e_ovno = this.USHORT();		// Overlay number
	obj.e_res = [ this.USHORT(), this.USHORT(), this.USHORT(), this.USHORT() ]; // Reserved words
	obj.e_oemid = this.USHORT();	// OEM identifier (for e_oeminfo)
	obj.e_oeminfo = this.USHORT();	// OEM information; e_oemid specific
	obj.e_res2 = [
				this.USHORT(), this.USHORT(), this.USHORT(), this.USHORT(), this.USHORT(),
				this.USHORT(), this.USHORT(), this.USHORT(), this.USHORT(), this.USHORT()
			];							// Reserved words
	obj.e_lfanew = this.LONG();		// File address of new exe header
	return obj;
}
WindowsExeFile.prototype.WindowsExeRead = function() {
	this.DosHeader 		= this.DosHeaderRead();			// Read the MSDOS 2 Legacy Header then Jump
	this.Position 		= this.DosHeader.e_lfanew;		// Set the position
	this.FileType 		= this.FileTypeRead();			// Read the file type information for NT PE
	this.FileHeader 	= this.FileHeaderRead();		// Read the file headers
	this.OptionalHeader = this.OptionalHeaderRead();	// Read the optional headers
	this.SectionHeaders = this.SectionHeadersRead();	// Read the section headers
	
	this.ResourcePosition = WindowsConst.GETOFFSETBYDIRECTORY(WindowsConst.IMAGE_DIRECTORY_ENTRY_RESOURCE, this);
	this.Position 		= this.ResourcePosition;
	this.Resources 		= this.ResourceDirectoryRead(this);	// Read resource headers
	delete this.ResourcePosition;	
}
var PngConsts = {
    PNG_SIGNATURE: [0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a],
    TYPE_IHDR: 0x49484452,
    TYPE_IEND: 0x49454e44,
    TYPE_IDAT: 0x49444154,
    TYPE_PLTE: 0x504c5445,
    TYPE_tRNS: 0x74524e53,
    TYPE_gAMA: 0x67414d41,
    COLOR_PALETTE: 1,
    COLOR_COLOR: 2,
    COLOR_ALPHA: 4
}
var ChunkStream = function() {
    Stream.call(this);
    this._buffers = [];
    this._buffered = 0;
    this._reads = [];
    this._paused = false;
    this._encoding = 'utf8';
    this.writable = true;
};
util.inherits(ChunkStream, Stream);
ChunkStream.prototype.read = function(length, callback) {
    this._reads.push({
        length: Math.abs(length),  // if length < 0 then at most this length
        allowLess: length < 0,
        func: callback
    });
    this._process();
    if (this._paused && this._reads.length > 0) {
        this._paused = false;
        this.emit('drain');
    }
};
ChunkStream.prototype.write = function(data, encoding) {
    if (!this.writable) {
        this.emit('error', new Error('Stream not writable'));
        return false;
    }
    if (!Buffer.isBuffer(data)) data = new Buffer(data, encoding || this._encoding);
    this._buffers.push(data);
    this._buffered += data.length;
    this._process();
    if (this._reads && this._reads.length == 0)
        this._paused = true;
    return this.writable && !this._paused;
};
ChunkStream.prototype.end = function(data, encoding) {
    if (data) this.write(data, encoding);
    this.writable = false;
    if (!this._buffers) return;
    if (this._buffers.length == 0) {
        this._end();
    } else {
        this._buffers.push(null);
        this._process();
    }
};
ChunkStream.prototype.destroySoon = ChunkStream.prototype.end;
ChunkStream.prototype._end = function() {
    if (this._reads.length > 0)
        this.emit('error',  new Error('There are some read requests waitng on finished stream'));
    this.destroy();
};
ChunkStream.prototype.destroy = function() {
    if (!this._buffers) return;
    this.writable = false;
    this._reads = null;
    this._buffers = null;
    this.emit('close');
};
ChunkStream.prototype._process = function() {
    while (this._buffered > 0 && this._reads && this._reads.length > 0) {
        var read = this._reads[0];
        if (read.allowLess) {
            this._reads.shift(); // == read
            var buf = this._buffers[0];
            if (buf.length > read.length) {
                this._buffered -= read.length;
                this._buffers[0] = buf.slice(read.length);
                read.func.call(this, buf.slice(0, read.length));
            } else {
                this._buffered -= buf.length;
                this._buffers.shift(); // == buf
                read.func.call(this, buf);
            }

        } else if (this._buffered >= read.length) {
            this._reads.shift(); // == read
            var pos = 0, count = 0, data = new Buffer(read.length);
            while (pos < read.length) {
                var buf = this._buffers[count++],
                    len = Math.min(buf.length, read.length - pos);
                buf.copy(data, pos, 0, len);
                pos += len;
                if (len != buf.length) this._buffers[--count] = buf.slice(len);
            }
            if (count > 0) this._buffers.splice(0, count);
            this._buffered -= read.length;
            read.func.call(this, data);
        } else {
            break;
        }
    }
    if (this._buffers && this._buffers.length > 0 && this._buffers[0] == null) {
        this._end();
    }
};
var CrcStream = function() {
    Stream.call(this);
    this._crc = -1;
    this.writable = true;
};
util.inherits(CrcStream, Stream);
CrcStream.prototype.write = function(data) {
    for (var i = 0; i < data.length; i++)
        this._crc = crcTable[(this._crc ^ data[i]) & 0xff] ^ (this._crc >>> 8);
    return true;
};
CrcStream.prototype.end = function(data) {
    if (data) this.write(data);
    this.emit('crc', this.crc32());
};
CrcStream.prototype.crc32 = function() { return this._crc ^ -1; };
CrcStream.crc32 = function(buf) {
    var crc = -1;
    for (var i = 0; i < buf.length; i++) {
        crc = crcTable[(crc ^ buf[i]) & 0xff] ^ (crc >>> 8);
    }
    return crc ^ -1;
};
var crcTable = [];
for (var i = 0; i < 256; i++) {
    var c = i;
    for (var j = 0; j < 8; j++) {
        if (c & 1)  c = 0xedb88320 ^ (c >>> 1);
       	else c = c >>> 1;
    }
    crcTable[i] = c;
}
var Filter = function(width, height, Bpp, data, options) {
    ChunkStream.call(this);
    this._width = width;
    this._height = height;
    this._Bpp = Bpp;
    this._data = data;
    this._options = options;
    this._line = 0;
    if (!('filterType' in options) || options.filterType == -1) options.filterType = [0, 1, 2, 3, 4];
    else if (typeof options.filterType == 'number') options.filterType = [options.filterType];
    this._filters = {
        0: this._filterNone.bind(this),
        1: this._filterSub.bind(this),
        2: this._filterUp.bind(this),
        3: this._filterAvg.bind(this),
        4: this._filterPaeth.bind(this)
    };
    this.read(this._width * Bpp + 1, this._reverseFilterLine.bind(this));
};
util.inherits(Filter, ChunkStream);
var pixelBppMap = {
    1: { // L
        0: 0,
        1: 0,
        2: 0,
        3: 0xff
    },
    2: { // LA
        0: 0,
        1: 0,
        2: 0,
        3: 1
    },
    3: { // RGB
        0: 0,
        1: 1,
        2: 2,
        3: 0xff
    },
    4: { // RGBA
        0: 0,
        1: 1,
        2: 2,
        3: 3
    }
};
Filter.prototype._reverseFilterLine = function(rawData) {
    var pxData = this._data, pxLineLength = this._width << 2, pxRowPos = this._line * pxLineLength, filter = rawData[0];
    if (filter == 0) {
        for (var x = 0; x < this._width; x++) {
            var pxPos = pxRowPos + (x << 2), rawPos = 1 + x * this._Bpp;
            for (var i = 0; i < 4; i++) {
                var idx = pixelBppMap[this._Bpp][i];
                pxData[pxPos + i] = idx != 0xff ? rawData[rawPos + idx] : 0xff;
            }
        }

    } else if (filter == 1) {
        for (var x = 0; x < this._width; x++) {
            var pxPos = pxRowPos + (x << 2), rawPos = 1 + x * this._Bpp;
            for (var i = 0; i < 4; i++) {
                var idx = pixelBppMap[this._Bpp][i], left = x > 0 ? pxData[pxPos + i - 4] : 0;
                pxData[pxPos + i] = idx != 0xff ? rawData[rawPos + idx] + left : 0xff;
            }
        }
    } else if (filter == 2) {
        for (var x = 0; x < this._width; x++) {
            var pxPos = pxRowPos + (x << 2), rawPos = 1 + x * this._Bpp;
            for (var i = 0; i < 4; i++) {
                var idx = pixelBppMap[this._Bpp][i],
                    up = this._line > 0 ? pxData[pxPos - pxLineLength + i] : 0;
                pxData[pxPos + i] = idx != 0xff ? rawData[rawPos + idx] + up : 0xff;
            }
        }
    } else if (filter == 3) {
        for (var x = 0; x < this._width; x++) {
            var pxPos = pxRowPos + (x << 2), rawPos = 1 + x * this._Bpp;
            for (var i = 0; i < 4; i++) {
                var idx = pixelBppMap[this._Bpp][i],
                    left = x > 0 ? pxData[pxPos + i - 4] : 0,
                    up = this._line > 0 ? pxData[pxPos - pxLineLength + i] : 0,
                    add = Math.floor((left + up) / 2);
                 pxData[pxPos + i] = idx != 0xff ? rawData[rawPos + idx] + add : 0xff;
            }
        }
    } else if (filter == 4) {
        for (var x = 0; x < this._width; x++) {
            var pxPos = pxRowPos + (x << 2), rawPos = 1 + x * this._Bpp;
            for (var i = 0; i < 4; i++) {
                var idx = pixelBppMap[this._Bpp][i],
                    left = x > 0 ? pxData[pxPos + i - 4] : 0,
                    up = this._line > 0 ? pxData[pxPos - pxLineLength + i] : 0,
                    upLeft = x > 0 && this._line > 0 ? pxData[pxPos - pxLineLength + i - 4] : 0,
                    add = PaethPredictor(left, up, upLeft);
                pxData[pxPos + i] = idx != 0xff ? rawData[rawPos + idx] + add : 0xff;
            }
        }
    }
    this._line++;
    if (this._line < this._height) this.read(this._width * this._Bpp + 1, this._reverseFilterLine.bind(this));
    else this.emit('complete', this._data, this._width, this._height);
};
Filter.prototype.filter = function() {
    var pxData = this._data, rawData = new Buffer(((this._width << 2) + 1) * this._height);
    for (var y = 0; y < this._height; y++) {
        var filterTypes = this._options.filterType, min = Infinity, sel = 0;

        for (var i = 0; i < filterTypes.length; i++) {
            var sum = this._filters[filterTypes[i]](pxData, y, null);
            if (sum < min) {
                sel = filterTypes[i];
                min = sum;
            }
        }
        this._filters[sel](pxData, y, rawData);
    }
    return rawData;
};
Filter.prototype._filterNone = function(pxData, y, rawData) {
    var pxRowLength = this._width << 2, rawRowLength = pxRowLength + 1, sum = 0;
    if (!rawData) {
        for (var x = 0; x < pxRowLength; x++) sum += Math.abs(pxData[y * pxRowLength + x]);
    } else {
        rawData[y * rawRowLength] = 0;
        pxData.copy(rawData, rawRowLength * y + 1, pxRowLength * y, pxRowLength * (y + 1));
    }
    return sum;
};
Filter.prototype._filterSub = function(pxData, y, rawData) {
    var pxRowLength = this._width << 2, rawRowLength = pxRowLength + 1, sum = 0;
    if (rawData) rawData[y * rawRowLength] = 1;
    for (var x = 0; x < pxRowLength; x++) {
        var left = x >= 4 ? pxData[y * pxRowLength + x - 4] : 0, val = pxData[y * pxRowLength + x] - left;
        if (!rawData) sum += Math.abs(val);
        else rawData[y * rawRowLength + 1 + x] = val;
    }
    return sum;
};
Filter.prototype._filterUp = function(pxData, y, rawData) {
    var pxRowLength = this._width << 2, rawRowLength = pxRowLength + 1, sum = 0;
    if (rawData) rawData[y * rawRowLength] = 2;
    for (var x = 0; x < pxRowLength; x++) {
        var up = y > 0 ? pxData[(y - 1) * pxRowLength + x] : 0, val = pxData[y * pxRowLength + x] - up;
        if (!rawData) sum += Math.abs(val);
        else rawData[y * rawRowLength + 1 + x] = val;
    }
    return sum;
};
Filter.prototype._filterAvg = function(pxData, y, rawData) {
    var pxRowLength = this._width << 2, rawRowLength = pxRowLength + 1, sum = 0;
    if (rawData) rawData[y * rawRowLength] = 3;
    for (var x = 0; x < pxRowLength; x++) {
        var left = x >= 4 ? pxData[y * pxRowLength + x - 4] : 0,
            up = y > 0 ? pxData[(y - 1) * pxRowLength + x] : 0,
            val = pxData[y * pxRowLength + x] - ((left + up) >> 1);
        if (!rawData) sum += Math.abs(val);
        else rawData[y * rawRowLength + 1 + x] = val;
    }
    return sum;
};
Filter.prototype._filterPaeth = function(pxData, y, rawData) {
    var pxRowLength = this._width << 2, rawRowLength = pxRowLength + 1, sum = 0;
    if (rawData) rawData[y * rawRowLength] = 4;
    for (var x = 0; x < pxRowLength; x++) {
        var left = x >= 4 ? pxData[y * pxRowLength + x - 4] : 0,
            up = y > 0 ? pxData[(y - 1) * pxRowLength + x] : 0,
            upLeft = x >= 4 && y > 0 ? pxData[(y - 1) * pxRowLength + x - 4] : 0,
            val = pxData[y * pxRowLength + x] - PaethPredictor(left, up, upLeft);

        if (!rawData) sum += Math.abs(val);
        else rawData[y * rawRowLength + 1 + x] = val;
    }
    return sum;
};
var PaethPredictor = function(left, above, upLeft) {
    var p = left + above - upLeft,
        pLeft = Math.abs(p - left),
        pAbove = Math.abs(p - above),
        pUpLeft = Math.abs(p - upLeft);
    if (pLeft <= pAbove && pLeft <= pUpLeft) return left;
    else if (pAbove <= pUpLeft) return above;
    else return upLeft;
};
var Packer = function(options) {
    Stream.call(this);
    this._options = options;
    options.deflateChunkSize = options.deflateChunkSize || 32 * 1024;
    options.deflateLevel = options.deflateLevel || 9;
    options.deflateStrategy = options.deflateStrategy || 3;
    this.readable = true;
};
util.inherits(Packer, Stream);
Packer.prototype.pack = function(data, width, height) {
    this.emit('data', new Buffer(PngConsts.PNG_SIGNATURE));
    this.emit('data', this._packIHDR(width, height));
    var filter = new Filter(width, height, 4, data, this._options);
    var data = filter.filter();
    var deflate = zlib.createDeflate({
            chunkSize: this._options.deflateChunkSize,
            level: this._options.deflateLevel,
            strategy: this._options.deflateStrategy
        });
    deflate.on('error', this.emit.bind(this, 'error'));
    deflate.on('data', function(data) { this.emit('data', this._packIDAT(data)); }.bind(this));
    deflate.on('end', function() { this.emit('data', this._packIEND());  this.emit('end');}.bind(this));
    deflate.end(data);
};
Packer.prototype._packChunk = function(type, data) {
    var len = (data ? data.length : 0), buf = new Buffer(len + 12);
    buf.writeUInt32BE(len, 0);
    buf.writeUInt32BE(type, 4);
    if (data) data.copy(buf, 8);
    buf.writeInt32BE(CrcStream.crc32(buf.slice(4, buf.length - 4)), buf.length - 4);
    return buf;
};
Packer.prototype._packIHDR = function(width, height) {
    var buf = new Buffer(13);
    buf.writeUInt32BE(width, 0);
    buf.writeUInt32BE(height, 4);
    buf[8] = 8;
    buf[9] = 6; // colorType
    buf[10] = 0; // compression
    buf[11] = 0; // filter
    buf[12] = 0; // interlace
    return this._packChunk(PngConsts.TYPE_IHDR, buf);
};
Packer.prototype._packIDAT = function(data) { return this._packChunk(PngConsts.TYPE_IDAT, data); };
Packer.prototype._packIEND = function() { return this._packChunk(PngConsts.TYPE_IEND, null); };
var Parser = function(options) {
    ChunkStream.call(this);
    this._options = options;
    options.checkCRC = options.checkCRC !== false;
    this._hasIHDR = false;
    this._hasIEND = false;
    this._inflate = null;
    this._filter = null;
    this._crc = null;
    this._palette = [];
    this._colorType = 0;
    this._chunks = {};
    this._chunks[PngConsts.TYPE_IHDR] = this._handleIHDR.bind(this);
    this._chunks[PngConsts.TYPE_IEND] = this._handleIEND.bind(this);
    this._chunks[PngConsts.TYPE_IDAT] = this._handleIDAT.bind(this);
    this._chunks[PngConsts.TYPE_PLTE] = this._handlePLTE.bind(this);
    this._chunks[PngConsts.TYPE_tRNS] = this._handleTRNS.bind(this);
    this._chunks[PngConsts.TYPE_gAMA] = this._handleGAMA.bind(this);
    this.writable = true;
    this.on('error', this._handleError.bind(this));
    this._handleSignature();
};
util.inherits(Parser, ChunkStream);
Parser.prototype._handleError = function() {
    this.writable = false;
    this.destroy();
    if (this._inflate) this._inflate.destroy();
};
Parser.prototype._handleSignature = function() { this.read(PngConsts.PNG_SIGNATURE.length, this._parseSignature.bind(this));};
Parser.prototype._parseSignature = function(data) {
    var signature = PngConsts.PNG_SIGNATURE;
    for (var i = 0; i < signature.length; i++) {
        if (data[i] != signature[i]) {
            this.emit('error', new Error('Invalid file signature'));
            return;
        }
    }
    this.read(8, this._parseChunkBegin.bind(this));
};
Parser.prototype._parseChunkBegin = function(data) {
    var length = data.readUInt32BE(0);
    var type = data.readUInt32BE(4), name = '';
    for (var i = 4; i < 8; i++) name += String.fromCharCode(data[i]);
    var ancillary = !!(data[4] & 0x20), priv= !!(data[5] & 0x20), safeToCopy = !!(data[7] & 0x20);
    if (!this._hasIHDR && type != PngConsts.TYPE_IHDR) {
        this.emit('error', new Error('Expected IHDR on beggining'));
        return;
    }
    this._crc = new CrcStream();
    this._crc.write(new Buffer(name));
    if (this._chunks[type]) return this._chunks[type](length);
	else if (!ancillary) this.emit('error', new Error('Unsupported critical chunk type ' + name));
    else this.read(length + 4, this._skipChunk.bind(this));
};
Parser.prototype._skipChunk = function(data) {this.read(8, this._parseChunkBegin.bind(this));};
Parser.prototype._handleChunkEnd = function() { this.read(4, this._parseChunkEnd.bind(this));};
Parser.prototype._parseChunkEnd = function(data) {
    var fileCrc = data.readInt32BE(0), calcCrc = this._crc.crc32();
    if (this._options.checkCRC && calcCrc != fileCrc) {
        this.emit('error', new Error('Crc error'));
        return;
    }
    if (this._hasIEND) this.destroySoon();
	else this.read(8, this._parseChunkBegin.bind(this));
};
Parser.prototype._handleIHDR = function(length) { this.read(length, this._parseIHDR.bind(this));};
Parser.prototype._parseIHDR = function(data) {
    this._crc.write(data);
    var width = data.readUInt32BE(0), height = data.readUInt32BE(4), depth = data[8],
        colorType = data[9], compr = data[10], filter = data[11], interlace = data[12];
    if (depth != 8) {
        this.emit('error', new Error('Unsupported bit depth ' + depth));
        return;
    }
    if (!(colorType in colorTypeToBppMap)) {
        this.emit('error', new Error('Unsupported color type'));
        return;
    }
    if (compr != 0) {
        this.emit('error', new Error('Unsupported compression method'));
        return;
    }
    if (filter != 0) {
        this.emit('error', new Error('Unsupported filter method'));
        return;
    }
    if (interlace != 0) {
        this.emit('error', new Error('Unsupported interlace method'));
        return;
    }
    this._colorType = colorType;
    this._data = new Buffer(width * height * 4);
    this._filter = new Filter(
        width, height,
        colorTypeToBppMap[this._colorType],
        this._data,
        this._options
    );
    this._hasIHDR = true;
    this.emit('metadata', {
        width: width,
        height: height,
        palette: !!(colorType & PngConsts.COLOR_PALETTE),
        color: !!(colorType & PngConsts.COLOR_COLOR),
        alpha: !!(colorType & PngConsts.COLOR_ALPHA),
        data: this._data
    });
    this._handleChunkEnd();
};
Parser.prototype._handlePLTE = function(length) { this.read(length, this._parsePLTE.bind(this)); };
Parser.prototype._parsePLTE = function(data) {
    this._crc.write(data);
    var entries = Math.floor(data.length / 3);
    for (var i = 0; i < entries; i++)
        this._palette.push([ data.readUInt8(i * 3), data.readUInt8(i * 3 + 1), data.readUInt8(i * 3 + 2 ), 0xff]);
    this._handleChunkEnd();
};
Parser.prototype._handleTRNS = function(length) { this.read(length, this._parseTRNS.bind(this)); };
Parser.prototype._parseTRNS = function(data) {
    this._crc.write(data);
    if (this._colorType == 3) {
        if (this._palette.length == 0) {
            this.emit('error', new Error('Transparency chunk must be after palette'));
            return;
        }
        if (data.length > this._palette.length) {
            this.emit('error', new Error('More transparent colors than palette size'));
            return;
        }
        for (var i = 0; i < this._palette.length; i++) { this._palette[i][3] = i < data.length ? data.readUInt8(i) : 0xff; }
    }
    this._handleChunkEnd();
};
Parser.prototype._handleGAMA = function(length) { this.read(length, this._parseGAMA.bind(this)); };
Parser.prototype._parseGAMA = function(data) {
    this._crc.write(data);
    this.emit('gamma', data.readUInt32BE(0) / 100000);
    this._handleChunkEnd();
};
Parser.prototype._handleIDAT = function(length) { this.read(-length, this._parseIDAT.bind(this, length)); };
Parser.prototype._parseIDAT = function(length, data) {
    this._crc.write(data);
    if (this._colorType == 3 && this._palette.length == 0) throw new Error('Expected palette not found');
    if (!this._inflate) {
        this._inflate = zlib.createInflate();
        this._inflate.on('error', this.emit.bind(this, 'error'));
        this._filter.on('complete', this._reverseFiltered.bind(this));
        this._inflate.pipe(this._filter);
    }
    this._inflate.write(data);
    length -= data.length;
    if (length > 0) this._handleIDAT(length); else this._handleChunkEnd();
};
Parser.prototype._handleIEND = function(length) { this.read(length, this._parseIEND.bind(this)); };
Parser.prototype._parseIEND = function(data) {
    this._crc.write(data);
    this._inflate.end();
    this._hasIEND = true;
    this._handleChunkEnd();
};
var colorTypeToBppMap = { 0: 1, 2: 3, 3: 1, 4: 2, 6: 4 };
Parser.prototype._reverseFiltered = function(data, width, height) {
    if (this._colorType == 3) {
        var pxLineLength = width << 2;
        for (var y = 0; y < height; y++) {
            var pxRowPos = y * pxLineLength;
            for (var x = 0; x < width; x++) {
                var pxPos = pxRowPos + (x << 2), color = this._palette[data[pxPos]];
                for (var i = 0; i < 4; i++) data[pxPos + i] = color[i];
            }
        }
    }
    this.emit('parsed', data);
};
var PNG = function(options) {
    Stream.call(this);
    options = options || {};
    this.width = options.width || 0;
    this.height = options.height || 0;
    this.data = this.width > 0 && this.height > 0 ? new Buffer(4 * this.width * this.height) : null;
    this.gamma = 0;
    this.readable = this.writable = true;
    this._parser = new Parser(options || {});
    this._parser.on('error', this.emit.bind(this, 'error'));
    this._parser.on('close', this._handleClose.bind(this));
    this._parser.on('metadata', this._metadata.bind(this));
    this._parser.on('gamma', this._gamma.bind(this));
    this._parser.on('parsed', function(data) { this.data = data; this.emit('parsed', data);}.bind(this));
    this._packer = new Packer(options);
    this._packer.on('data', this.emit.bind(this, 'data'));
    this._packer.on('end', this.emit.bind(this, 'end'));
    this._parser.on('close', this._handleClose.bind(this));
    this._packer.on('error', this.emit.bind(this, 'error'));
};
util.inherits(PNG, Stream);
PNG.prototype.pack = function() {
    process.nextTick(function() { this._packer.pack(this.data, this.width, this.height); }.bind(this));
    return this;
};
PNG.prototype.parse = function(data, callback) {
    if (callback) {
        var onParsed = null, onError = null;
        this.once('parsed', onParsed = function(data) {
            this.removeListener('error', onError);
            this.data = data;
            callback(null, this);
        }.bind(this));
        this.once('error', onError = function(err) {
            this.removeListener('parsed', onParsed);
            callback(err, null);
        }.bind(this));
    }
    this.end(data);
    return this;
};
PNG.prototype.write = function(data) {
    this._parser.write(data);
    return true;
};
PNG.prototype.end = function(data) { this._parser.end(data); };
PNG.prototype._metadata = function(metadata) {
    this.width = metadata.width;
    this.height = metadata.height;
    this.data = metadata.data;
    delete metadata.data;
    this.emit('metadata', metadata);
};
PNG.prototype._gamma = function(gamma) { this.gamma = gamma; };
PNG.prototype._handleClose = function() { if (!this._parser.writable && !this._packer.readable)  this.emit('close');};
PNG.prototype.bitblt = function(dst, sx, sy, w, h, dx, dy) {
    var src = this;
    if (sx > src.width || sy > src.height || sx + w > src.width || sy + h > src.height) throw new Error('bitblt reading outside image');
    if (dx > dst.width || dy > dst.height || dx + w > dst.width || dy + h > dst.height) throw new Error('bitblt writing outside image');
    for (var y = 0; y < h; y++) {
        src.data.copy(dst.data, ((dy + y) * dst.width + dx) << 2, ((sy + y) * src.width + sx) << 2, ((sy + y) * src.width + sx + w) << 2 );
    }
    return this;
};