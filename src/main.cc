#include <napi.h>
#ifdef __WIN32__
#include "clip_win.h"
#elif __APPLE__
#include "clip_osx.h"
#endif

Napi::Value get_file_names(const Napi::CallbackInfo &info);

Napi::Value write_file_names(const Napi::CallbackInfo &info);

static Napi::Value readFiles(const Napi::CallbackInfo &info)
{
	return get_file_names(info);
}
static Napi::Value writeFiles(const Napi::CallbackInfo &info)
{
	return write_file_names(info);
}
static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
	exports["readFiles"] = Napi::Function::New(env, readFiles);
	exports["writeFiles"] = Napi::Function::New(env, writeFiles);
	return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)