#include <windows.h>
#include <napi.h>
#include <shlobj.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

Napi::Value get_file_names(const Napi::CallbackInfo &info);

Napi::Value write_file_names(const Napi::CallbackInfo &info);