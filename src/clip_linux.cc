#include "clip.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>

std::string URIDecode(const std::string& str) {
    std::string decoded;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            char hex[3] = { str[i + 1], str[i + 2], '\0' };
            char* end;
            long value = strtol(hex, &end, 16);
            
            if (end == hex + 2 && value >= 0 && value <= 255) {
                decoded += static_cast<char>(value);
                i += 2;
            } else {
                decoded += str[i];
            }
        } else {
            decoded += str[i];
        }
    }
    
    return decoded;
}

std::vector<std::string> URIListToFilePaths(const std::string& uri_list) {
    std::vector<std::string> file_paths;
    std::istringstream stream(uri_list);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        const std::string file_prefix = "file://";
        if (line.compare(0, file_prefix.length(), file_prefix) == 0) {
            std::string path = line.substr(file_prefix.length());
            
            size_t first_slash = path.find('/');
            if (first_slash != 0 && first_slash != std::string::npos) {
                path = path.substr(first_slash);
            }
            
            path = URIDecode(path);
            
            if (!path.empty()) {
                file_paths.push_back(path);
            }
        }
    }
    
    return file_paths;
}

Napi::Array GetFileNames(Napi::Env env) {
    Napi::Array result = Napi::Array::New(env);
    uint32_t index = 0;
    
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        return result;
    }
    
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                       0, 0, 1, 1, 0, 0, 0);
    
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom uriList = XInternAtom(display, "text/uri-list", False);
    Atom property = XInternAtom(display, "CLIPBOARD_DATA", False);
    
    XConvertSelection(display, clipboard, uriList, property, window, CurrentTime);
    XFlush(display);
    
    XEvent event;
    bool success = false;
    
    for (int i = 0; i < 100; ++i) {
        if (XCheckTypedWindowEvent(display, window, SelectionNotify, &event)) {
            success = true;
            break;
        }
        usleep(1000);
    }
    
    if (success && event.xselection.property != None) {
        Atom actual_type;
        int actual_format;
        unsigned long nitems, bytes_after;
        unsigned char* data = nullptr;
        
        int result_code = XGetWindowProperty(
            display, window, property,
            0, (~0L),
            False, AnyPropertyType,
            &actual_type, &actual_format,
            &nitems, &bytes_after, &data);
        
        if (result_code == Success && data) {
            std::string uri_list(reinterpret_cast<char*>(data), nitems);
            XFree(data);
            
            std::vector<std::string> file_paths = URIListToFilePaths(uri_list);

            for (const auto& path : file_paths) {
                result.Set(index++, Napi::String::New(env, path));
            }
        }
    }
    
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return result;
}

void WriteFileNames(Napi::Env env, Napi::Array files) {
    return;
}
