#import "content/nw/src/api/base/base_mac.h"
#import <objc/runtime.h>

#include "base/memory/raw_ptr_exclusion.h"

@interface CppWrapper : NSObject {
  RAW_PTR_EXCLUSION void* _obj;
}
+ (id) createFromCppObject:(void*) obj;
- (id) initWithCppObject:(void*) obj;
- (void*) obj;
@end

@implementation CppWrapper

+ (id) createFromCppObject:(void*) obj {
  return [[CppWrapper alloc] initWithCppObject:obj];
}

- (id) initWithCppObject:(void*) obj {
  _obj = obj;
  return self;
}

- (void*) obj {
  return _obj;
}

@end

@implementation NSObject (AssociatedObject)
@dynamic associatedObject;

- (void)setAssociatedObject:(void*)object {
  objc_setAssociatedObject(self, @selector(associatedObject), [CppWrapper createFromCppObject:object], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (void*)associatedObject {
  CppWrapper* wrapper = objc_getAssociatedObject(self, @selector(associatedObject));
  return wrapper == nil ? nil : [wrapper obj];
}

@end
