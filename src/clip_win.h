#include <windows.h>
#include <node.h>
#include <shlobj.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using v8::Context;
using v8::Array;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::String;
using v8::NewStringType;
using v8::Value;

Local<Array> get_file_names(Isolate *isolate);

void write_file_names(Isolate *isolate, Local<Array> fileNames);