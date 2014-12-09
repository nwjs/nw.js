// Copyright (c) 2014 Jefry Tedjokusumo <jtg_gg@yahoo.com.sg>
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


#ifndef CONTENT_NW_SRC_API_EVENT_EVENT_H_
#define CONTENT_NW_SRC_API_EVENT_EVENT_H_


#include "base/basictypes.h"

#include "content/nw/src/api/base/base.h"
#include "ui/gfx/display_observer.h"

#include <map>

namespace nwapi {
  
class BaseEvent {
  friend class EventListener;
  DISALLOW_COPY_AND_ASSIGN(BaseEvent);
  
protected:
  BaseEvent(){}
  virtual ~BaseEvent(){}
};
  
class EventListener : public Base {
  std::map<int, BaseEvent*> listerners_;
  
public:
  EventListener(int id,
                const base::WeakPtr<DispatcherHost>& dispatcher_host,
                const base::DictionaryValue& option);
  
  virtual ~EventListener();
  
  static int getUID() {
    static int id = 0;
    return ++id;
  }
  
  template<typename T> T* AddListener() {
    std::map<int, BaseEvent*>::iterator i = listerners_.find(T::id);
    if (i==listerners_.end()) {
      T* listener_object = new T(this);
      listerners_[T::id] = listener_object;
      return listener_object;
    }
    return NULL;
  }
  
  template<typename T> bool RemoveListener() {
    std::map<int, BaseEvent*>::iterator i = listerners_.find(T::id);
    if (i!=listerners_.end()) {
      delete i->second;
      listerners_.erase(i);
      return true;
    }
    return false;
  }
private:
  DISALLOW_COPY_AND_ASSIGN(EventListener);
};

} // namespace nwapi

#endif //CONTENT_NW_SRC_API_EVENT_EVENT_H_
