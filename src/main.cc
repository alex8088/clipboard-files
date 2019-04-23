#include <node.h>
#ifdef __WIN32__
#include "clip_win.h"
#elif __APPLE__
#include "clip_osx.h"
#endif

namespace clipboard
{
	using v8::Array;
	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Object;
	using v8::String;
	using v8::Value;

	void readFiles(const FunctionCallbackInfo<Value> &args)
	{
		Isolate *isolate = args.GetIsolate();
		Local<Array> fileNames = get_file_names(isolate);
		args.GetReturnValue().Set(fileNames);
	}
	void Init(Local<Object> exports)
	{
		NODE_SET_METHOD(exports, "readFiles", readFiles);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, Init)
}