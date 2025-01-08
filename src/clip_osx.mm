#import <Cocoa/Cocoa.h>
#include "clip_osx.h"

Napi::Array GetFileNames(Napi::Env env) {
    Napi::Array result = Napi::Array::New(env);
    uint32_t index = 0;
    
    @autoreleasepool {
        NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
        NSArray *classes = @[[NSURL class]];
        NSDictionary *options = @{NSPasteboardURLReadingFileURLsOnlyKey: @YES};
        
        NSArray *urls = [pasteboard readObjectsForClasses:classes options:options];
        if (urls) {
            for (NSURL *url in urls) {
                if ([url isFileURL]) {
                    NSString *path = [url path];
                    result.Set(index++, Napi::String::New(env, [path UTF8String]));
                }
            }
        }
    }
    
    return result;
}

void WriteFileNames(Napi::Env env, Napi::Array files) {
    @autoreleasepool {
        NSMutableArray *urls = [NSMutableArray array];
        uint32_t length = files.Length();
        
        for (uint32_t i = 0; i < length; i++) {
            Napi::Value value = files[i];
            if (value.IsString()) {
                std::string utf8Path = value.As<Napi::String>();
                NSString *path = [NSString stringWithUTF8String:utf8Path.c_str()];
                NSURL *url = [NSURL fileURLWithPath:path];
                if (url) {
                    [urls addObject:url];
                }
            }
        }
        
        if ([urls count] > 0) {
            NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
            [pasteboard clearContents];
            [pasteboard writeObjects:urls];
        }
    }
}
