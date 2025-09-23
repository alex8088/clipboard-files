#include "clip.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <vector>
#include <string>

Napi::Array GetFileNames(Napi::Env env) {
    Napi::Array result = Napi::Array::New(env);
    uint32_t index = 0;
    
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        return result;
    }
    
    Window window = DefaultRootWindow(display);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom uriList = XInternAtom(display, "text/uri-list", False);
    
    XEvent event;
    Atom type;
    int format;
    unsigned long len, bytes_left;
    unsigned char* data;
    
    XConvertSelection(display, clipboard, uriList, clipboard, window, CurrentTime);
    
    XFlush(display);
    
    XNextEvent(display, &event);
    if (event.type == SelectionNotify) {
        if (XGetWindowProperty(display, window, clipboard, 0, 1000000,
                             False, uriList, &type, &format, &len,
                             &bytes_left, &data) == Success) {
            if (data) {
                char* p = (char*)data;
                std::string uri;
                
                while (*p) {
                    if (*p == '\n' || *p == '\r') {
                        if (!uri.empty()) {
                            if (uri.substr(0, 7) == "file://") {
                                std::string path = uri.substr(7);
                                result.Set(index++, Napi::String::New(env, path));
                            }
                            uri.clear();
                        }
                    } else {
                        uri += *p;
                    }
                    p++;
                }
                
                XFree(data);
            }
        }
    }
    
    XCloseDisplay(display);
    return result;
}

void WriteFileNames(Napi::Env env, Napi::Array files) {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        return;
    }
    
    Window window = DefaultRootWindow(display);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom uriList = XInternAtom(display, "text/uri-list", False);
    
    std::string data;
    uint32_t length = files.Length();
    
    for (uint32_t i = 0; i < length; i++) {
        Napi::Value value = files[i];
        if (value.IsString()) {
            std::string path = value.As<Napi::String>();
            data += "file://" + path + "\r\n";
        }
    }
    
    if (!data.empty()) {
        XChangeProperty(display, window, clipboard, uriList, 8,
                       PropModeReplace,
                       (unsigned char*)data.c_str(),
                       data.length());
        
        XSetSelectionOwner(display, clipboard, window, CurrentTime);
    }
    
    XCloseDisplay(display);
}
