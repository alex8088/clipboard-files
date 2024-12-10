#include "clip_osx.h"

Local<Array> get_file_names(Isolate *isolate){
  Local<Array> fileNames = Array::New(isolate);
  Local<Context> context = isolate->GetCurrentContext();

  NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
  NSArray* tempArray = [pasteboard pasteboardItems];
  int count = 0;
  for(NSPasteboardItem *tmpItem in tempArray){
    NSString *pathString = [tmpItem stringForType:@"public.file-url"];
    if (pathString && [pathString isKindOfClass:[NSString class]]) {
      NSURL *url = [NSURL URLWithString:pathString];
      NSString *realPathString = [url path];
      if (realPathString) {
        const char* str = [[NSString stringWithFormat:@"file://%@", realPathString] UTF8String];
        fileNames->Set(context, count, String::NewFromUtf8(isolate, str, NewStringType::kNormal).ToLocalChecked());
        count++;
      }
    }
  }
  return fileNames;
}

void write_file_names(Isolate *isolate, Local<Array> fileNames)
{
  Local<Context> context = isolate->GetCurrentContext();

  std::vector<string> files;
  for (size_t i = 0; i < fileNames->Length(); i++) {
    MaybeLocal<Value> maybeIndex = fileNames->Get(context, i);
    Local<Value> index = maybeIndex.ToLocalChecked();
    String::Utf8Value path(isolate, index);
    std::string pathStr(*path);
    files.push_back(pathStr);
  }

  NSMutableArray* fileList = [NSMutableArray arrayWithCapacity:files.size()];
  NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
  for (unsigned int i = 0; i < files.size(); i++)
    [fileList addObject:[NSString stringWithUTF8String:files[i].c_str()]];
  [pasteboard declareTypes:[NSArray arrayWithObject:NSFilenamesPboardType] owner:nil];
  [pasteboard setPropertyList:fileList forType:NSFilenamesPboardType];
}
