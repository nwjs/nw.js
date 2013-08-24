#ifndef CONTENT_TI_SRC_API_BINDINGS_H_
#define CONTENT_TI_SRC_API_BINDINGS_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "v8/include/v8.h"

namespace api {
  
  class TiAppBindings : public v8::Extension {
  public:
    TiAppBindings();
    virtual ~TiAppBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiAppBindings);
  };
  
  class TiBufferedStreamBindings : public v8::Extension {
  public:
    TiBufferedStreamBindings();
    virtual ~TiBufferedStreamBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiBufferedStreamBindings);
  };
  
  class TiClipboardBindings : public v8::Extension {
  public:
    TiClipboardBindings();
    virtual ~TiClipboardBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiClipboardBindings);
  };
  
  class TiCompressionBindings : public v8::Extension {
  public:
    TiCompressionBindings();
    virtual ~TiCompressionBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiCompressionBindings);
  };
  
  class TiDomainBindings : public v8::Extension {
  public:
    TiDomainBindings();
    virtual ~TiDomainBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiDomainBindings);
  };
  
  class TiFileBindings : public v8::Extension {
  public:
    TiFileBindings();
    virtual ~TiFileBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiFileBindings);
  };
  
  class TiFileSystemBindings : public v8::Extension {
  public:
    TiFileSystemBindings();
    virtual ~TiFileSystemBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiFileSystemBindings);
  };
  
  class TiHashBindings : public v8::Extension {
  public:
    TiHashBindings();
    virtual ~TiHashBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiHashBindings);
  };
  
  class TiMenuBindings : public v8::Extension {
  public:
    TiMenuBindings();
    virtual ~TiMenuBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiMenuBindings);
  };
  
  class TiMenuItemBindings : public v8::Extension {
  public:
    TiMenuItemBindings();
    virtual ~TiMenuItemBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiMenuItemBindings);
  };
  
  class TiPlatformBindings : public v8::Extension {
  public:
    TiPlatformBindings();
    virtual ~TiPlatformBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiPlatformBindings);
  };
  
  class TiPropertyObjectBindings : public v8::Extension {
  public:
    TiPropertyObjectBindings();
    virtual ~TiPropertyObjectBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiPropertyObjectBindings);
  };
  
  class TiSocketBindings : public v8::Extension {
  public:
    TiSocketBindings();
    virtual ~TiSocketBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiSocketBindings);
  };
	
	class TiConnectionBindings : public v8::Extension {
	public:
		TiConnectionBindings();
		virtual ~TiConnectionBindings();
		
		DISALLOW_COPY_AND_ASSIGN(TiConnectionBindings);
	};

	class TiFileDialogBindings : public v8::Extension {
	public:
		TiFileDialogBindings();
		virtual ~TiFileDialogBindings();
		
		DISALLOW_COPY_AND_ASSIGN(TiFileDialogBindings);
	};
	
  class TiWindowBindings : public v8::Extension {
  public:
    TiWindowBindings();
    virtual ~TiWindowBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiWindowBindings);
  };
  
  class TiTrayBindings : public v8::Extension {
  public:
    TiTrayBindings();
    virtual ~TiTrayBindings();
    
    DISALLOW_COPY_AND_ASSIGN(TiTrayBindings);
  };
}  // namespace api

#endif  // CONTENT_TI_SRC_API_BINDINGS_H_
