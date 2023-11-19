#include <nan.h>
#include <stdio.h>
using namespace Nan;
using namespace v8;

void buffer_delete_callback(char* data, void* the_vector) {
  fputs("DELETE CALLBACK\n", stderr);
}

NAN_METHOD(rotate) {
    char* buffer = (char*) node::Buffer::Data(info[0]->ToObject(v8::Isolate::GetCurrent()->GetCurrentContext()).ToLocalChecked());
    size_t size = node::Buffer::Length(info[0]);
    unsigned int rot = info[1]->Uint32Value(v8::Isolate::GetCurrent()->GetCurrentContext()).ToChecked();
   
    char * retval = new char[size];
    for(unsigned int i = 0; i < size; i++ ) {
        retval[i] = buffer[i] - rot;
        buffer[i] += rot;
    }   
    
    //info.GetReturnValue().Set(Nan::NewBuffer(retval, size, buffer_delete_callback, nullptr).ToLocalChecked());
    info.GetReturnValue().Set(Nan::NewBuffer(retval, size).ToLocalChecked());
}

NAN_MODULE_INIT(Init) {
   Nan::Set(target, New<String>("rotate").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(rotate)).ToLocalChecked());
}

NODE_MODULE(buffer_example, Init)
