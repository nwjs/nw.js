// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "base/threading/thread_restrictions.h"
#include "third_party/zlib/google/zip.h"
#include "third_party/zlib/zlib.h"
#include "content/nw/src/api/app/app.h"
#include "base/file_util.h"
#include "base/files/memory_mapped_file.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop.h"
#include "base/values.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/browser/net_disk_cache_remover.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/common/view_messages.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_process_host.h"
#if defined(OS_WIN)
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) _setmode(_fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

using base::MessageLoop;
using content::Shell;
using content::ShellBrowserContext;
using content::RenderProcessHost;

namespace api {

namespace {

int _defgzip(FILE *source, FILE *dest, int level)
{
    int ret = 0, flush = 0;
    unsigned have = 0;
    z_stream strm = z_stream();
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit2(&strm, level,Z_DEFLATED,16+MAX_WBITS,8,Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}

// Get render process host.
RenderProcessHost* GetRenderProcessHost() {
  RenderProcessHost* render_process_host = NULL;
  std::vector<Shell*> windows = Shell::windows();
  for (size_t i = 0; i < windows.size(); ++i) {
    if (!windows[i]->is_devtools()) {
      render_process_host = windows[i]->web_contents()->GetRenderProcessHost();
      break;
    }
  }

  return render_process_host;
}

void GetRenderProcessHosts(std::set<RenderProcessHost*>& rphs) {
  RenderProcessHost* render_process_host = NULL;
  std::vector<Shell*> windows = Shell::windows();
  for (size_t i = 0; i < windows.size(); ++i) {
    if (!windows[i]->is_devtools()) {
      render_process_host = windows[i]->web_contents()->GetRenderProcessHost();
      rphs.insert(render_process_host);
    }
  }
}

}  // namespace

// static
void App::Call(const std::string& method,
               const base::ListValue& arguments) {
  if (method == "Quit") {
    Quit();
    return;
  } else if (method == "CloseAllWindows") {
    CloseAllWindows();
    return;
  }
  NOTREACHED() << "Calling unknown method " << method << " of App";
}


// static
void App::Call(Shell* shell,
               const std::string& method,
               const base::ListValue& arguments,
               base::ListValue* result) {
  base::ThreadRestrictions::SetIOAllowed(true);
  if (method == "GetDataPath") {
    ShellBrowserContext* browser_context =
      static_cast<ShellBrowserContext*>(shell->web_contents()->GetBrowserContext());
    result->AppendString(browser_context->GetPath().value());
    return;
  }else if (method == "GetArgv") {
    nw::Package* package = shell->GetPackage();
    CommandLine* command_line = CommandLine::ForCurrentProcess();
    CommandLine::StringVector args = command_line->GetArgs();
    CommandLine::StringVector argv = command_line->original_argv();

    // Ignore first non-switch arg if it's not a standalone package.
    bool ignore_arg = !package->self_extract();
    for (unsigned i = 1; i < argv.size(); ++i) {
      if (ignore_arg && argv[i] == args[0]) {
        ignore_arg = false;
        continue;
      }

      result->AppendString(argv[i]);
    }

    return;
  } else if (method == "Zip") { 
    std::string zipdir;
    std::string zipfile;
    arguments.GetString(0,&zipdir);
    arguments.GetString(1,&zipfile);
    result->AppendBoolean(zip::Zip(base::FilePath::FromUTF8Unsafe(zipdir), base::FilePath::FromUTF8Unsafe(zipfile), true));
    return;
  } else if (method == "Unzip") {
    std::string zipfile;
    std::string zipdir;
    arguments.GetString(0,&zipfile);
    arguments.GetString(1,&zipdir);
    result->AppendBoolean(zip::Unzip(base::FilePath::FromUTF8Unsafe(zipfile), base::FilePath::FromUTF8Unsafe(zipdir)));
    return;
  } else if (method == "Gzip") {
	  std::string ssrc = "";
	  std::string sdst = "";
    
	  arguments.GetString(0,&ssrc);
	  arguments.GetString(1,&sdst);

    FILE *src = fopen(ssrc.c_str(), "r");
    if(src==NULL) {
      DLOG(ERROR) << "Cannot open for read " << ssrc << " IO ERROR: " << _strerror(NULL);
      result->AppendBoolean(false);
      return;
    }

    FILE *dst = fopen(sdst.c_str(), "w+");

    if(dst==NULL) {
      DLOG(ERROR) << "Cannot open for read/write/trunc " << ssrc << " IO ERROR: " << _strerror(NULL);
      result->AppendBoolean(false);
      fclose(src);
      return;
    }

    SET_BINARY_MODE(src);
    SET_BINARY_MODE(dst);

    if(_defgzip(src,dst,Z_DEFAULT_COMPRESSION) == Z_OK)
      result->AppendBoolean(true);
    else
      result->AppendBoolean(false);

    fflush(dst);
    fclose(src);
    fclose(dst);
    return;

   /*
   // Open file in text mode:
   if( fopen_s( &stream, "fread.out", "w+t" ) == 0 )
   {
      for ( i = 0; i < 25; i++ )
         list[i] = (char)('z' - i);
      // Write 25 characters to stream 
      numwritten = fwrite( list, sizeof( char ), 25, stream );
      printf( "Wrote %d items\n", numwritten );
      fclose( stream );

   }
   else
      printf( "Problem opening the file\n" );

   if( fopen_s( &stream, "fread.out", "r+t" ) == 0 )
   {
      // Attempt to read in 25 characters 
      numread = fread( list, sizeof( char ), 25, stream );
      printf( "Number of items read = %d\n", numread );
      printf( "Contents of buffer = %.25s\n", list );
      fclose( stream );
   }
   else
      printf( "File could not be opened\n" );
}

    base::FilePath src = base::FilePath::FromUTF8Unsafe(ssrc);
    base::MemoryMappedFile mmap_;
    z_stream strm;
    unsigned char *ptr;
    if (!mmap_.Initialize(base::FilePath::FromUTF8Unsafe(ssrc))) {
		  result->AppendBoolean(false);
		  return;
		}

    unsigned char *data = (unsigned char *)malloc(CHUNK);
    int data_size = 0;
		strm.total_in = strm.avail_in = mmap_.length();
		strm.next_in = const_cast<unsigned char *>(mmap_.data());
		strm.zalloc = Z_NULL, strm.zfree = Z_NULL, strm.opaque = Z_NULL;

		if (deflateInit(&strm, Z_BEST_COMPRESSION) != Z_OK) {
		  result->AppendBoolean(false);
		  return;
		}

		while(true) {
			strm.avail_out=CHUNK;
			strm.next_out=data+data_size;
			switch(deflate(&strm, Z_FINISH)) {
        case Z_OK:
        case Z_BUF_ERROR:
          data_size = strm.total_out;
          ptr = (unsigned char *)realloc(data, data_size + CHUNK);
          if(ptr == NULL) {
            deflateEnd(&strm);
            result->AppendBoolean(false);
            return;
          }
				  data = ptr;
          break;
        case Z_STREAM_END:
          data_size = strm.total_out;
          deflateEnd(&strm);

          file_util::WriteFile(base::FilePath::FromUTF8Unsafe(sdst),reinterpret_cast<const char *>(data),data_size);

          result->AppendBoolean(true);
          return;
          break;
        default:
          inflateEnd(&strm);
          result->AppendBoolean(false);
          break;
      }
    }


#if defined(OS_WIN)
    int src;
    int dst;
    _sopen_s(&src,ssrc.c_str(), _O_RDONLY, _SH_DENYNO, _S_IREAD);
	  _sopen_s(&dst,sdst.c_str(), _O_WRONLY|_O_CREAT|_O_TRUNC, _SH_DENYNO, _S_IWRITE);
#else
	  int src = open(ssrc.c_str(), O_RDONLY);
	  int dst = open(sdst.c_str(), O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif
	  gzFile zDst = gzdopen(dst, "w");
	  if(gzbuffer(zDst, 0x1000)==-1) {
		  result->AppendBoolean(false);
		  return;
	  }
	  if(gzsetparams(zDst, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY) != Z_OK) {
		  result->AppendBoolean(false);
		  return;
	  }
	  if(zDst == NULL || src == -1) {
		  result->AppendBoolean(false);
		  return;
	  }
#if defined(OS_WIN)
    while(bytes_read > 0 && bytes_written > 0) {
		  bytes_read = _read(src,&buffer,0x1000);
      bytes_written = gzwrite(zDst,&buffer,bytes_read);
    }
    _close(src);
#else
    while(bytes_read > 0 && bytes_written > 0) {
		  bytes_read = read(src,&buffer,0x1000);
      bytes_written = gzwrite(zDst,&buffer,bytes_read);
    }
    close(src);
#endif
	  gzclose(zDst);
	  result->AppendBoolean(bytes_read != -1 && bytes_written != -1);*/
	  return;
  } else if (method == "Ungzip") {
	  std::string ssrc;
	  std::string sdst;
	  unsigned char buffer[0x1000];
	  int bytes_read = 1, bytes_written = 1, eof = 0;
	  
	  arguments.GetString(0,&ssrc);
	  arguments.GetString(1,&sdst);
	
#if defined(OS_WIN)
    int src;
    int dst;
    _sopen_s(&src,ssrc.c_str(), _O_RDONLY, _SH_DENYNO, _S_IREAD);
	  _sopen_s(&dst,sdst.c_str(), _O_WRONLY|_O_CREAT|_O_TRUNC, _SH_DENYNO, _S_IWRITE);
#else
	  int src = open(ssrc.c_str(), O_RDONLY);
	  int dst = open(sdst.c_str(), O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif
	  gzFile zSrc = gzdopen(src, "r");
	  if(gzbuffer(zSrc, 0x1000)==-1) {
		  result->AppendBoolean(false);
		  return;
	  }
	  
	  if(zSrc == NULL || dst == -1) {
		  result->AppendBoolean(false);
		  return;
	  }
	  
	  while(eof == 0 &&
			bytes_read > 0 &&
			bytes_written > 0) {
		  bytes_read = gzread(zSrc,&buffer,0x1000);
#if defined(OS_WIN)
		  bytes_written = _write(dst,&buffer,bytes_read);
#else
		  bytes_written = write(dst,&buffer,bytes_read);
#endif
		  eof = gzeof(zSrc);
	  }
#if defined(OS_WIN)
	  _close(dst);
#else
	  close(dst);
#endif
	  gzclose(zSrc);
	  result->AppendBoolean(eof==1);
	  return;
  } else if (method == "ClearCache") {
    ClearCache(GetRenderProcessHost());
  } else if (method == "GetPackage") {
    result->AppendString(shell->GetPackage()->package_string());
    return;
  }

  NOTREACHED() << "Calling unknown sync method " << method << " of App";
}

// static
void App::CloseAllWindows(bool force) {
  std::vector<Shell*> windows = Shell::windows();

  for (size_t i = 0; i < windows.size(); ++i) {
    // Only send close event to browser windows, since devtools windows will
    // be automatically closed.
    if (!windows[i]->is_devtools()) {
      // If there is no js object bound to the window, then just close.
      if (force || windows[i]->ShouldCloseWindow())
        // we used to delete the Shell object here
        // but it should be deleted on native window destruction
        windows[i]->window()->Close();
    }
  }
  if (force) {
    // in a special force close case, since we're going to exit the
    // main loop soon, we should delete the shell object asap so the
    // render widget can be closed on the renderer side
    windows = Shell::windows();
    for (size_t i = 0; i < windows.size(); ++i) {
      if (!windows[i]->is_devtools())
        delete windows[i];
    }
  }
}

// static
void App::Quit(RenderProcessHost* render_process_host) {
  // Send the quit message.
  int no_use;
  if (render_process_host) {
    render_process_host->Send(new ViewMsg_WillQuit(&no_use));
  }else{
    std::set<RenderProcessHost*> rphs;
    std::set<RenderProcessHost*>::iterator it;

    GetRenderProcessHosts(rphs);
    for (it = rphs.begin(); it != rphs.end(); it++) {
      RenderProcessHost* rph = *it;
      DCHECK(rph != NULL);

      rph->Send(new ViewMsg_WillQuit(&no_use));
    }
    CloseAllWindows(true);
  }
  // Then quit.
  MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
}

// static
void App::EmitOpenEvent(const std::string& path) {
  std::set<RenderProcessHost*> rphs;
  std::set<RenderProcessHost*>::iterator it;

  GetRenderProcessHosts(rphs);
  for (it = rphs.begin(); it != rphs.end(); it++) {
    RenderProcessHost* rph = *it;
    DCHECK(rph != NULL);

    rph->Send(new ShellViewMsg_Open(path));
  }
}

void App::ClearCache(content::RenderProcessHost* render_process_host) {
  render_process_host->Send(new ShellViewMsg_ClearCache());
  nw::RemoveHttpDiskCache(render_process_host->GetBrowserContext(),
                          render_process_host->GetID());
}

}  // namespace api
