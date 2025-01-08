#include <napi.h>
#ifdef __WIN32__
#include "clip_win.h"
#elif __APPLE__
#include "clip_osx.h"
#endif

Napi::Array ReadFiles(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return GetFileNames(env);
}

void WriteFiles(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsArray()) {
        return;
    }
    
    Napi::Array files = info[0].As<Napi::Array>();
    WriteFileNames(env, files);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("readFiles", Napi::Function::New(env, ReadFiles));
    exports.Set("writeFiles", Napi::Function::New(env, WriteFiles));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
