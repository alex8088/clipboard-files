#include "clip_osx.h"

Local<Array> get_file_names(Isolate *isolate){
  NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];  
  NSArray* tempArray = [pasteboard pasteboardItems];
  int count = 0;
  for(NSPasteboardItem *tmpItem in tempArray){ 
    NSString *pathString = [tmpItem stringForType:@"public.file-url"];
    const char* str = [pathString UTF8String];
    if(str){
      count++;
    }
  }
  Local<Array> fileNames = Array::New(isolate, count);
  count =0;
  for(NSPasteboardItem *tmpItem in tempArray){ 
    NSString *pathString = [tmpItem stringForType:@"public.file-url"];
    const char* str = [pathString UTF8String];
    if(str){
      fileNames->Set(count,String::NewFromUtf8(isolate,str));
      count++;
    }
  }
  return fileNames;
}
