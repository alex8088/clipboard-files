#include "clip_win.h"

std::wstring Utf8ToWide(const std::string& str) {
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}

std::string WideToUtf8(const std::wstring& wstr) {
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &str[0], count, NULL, NULL);
    return str;
}

Napi::Array GetFileNames(Napi::Env env) {
    Napi::Array result = Napi::Array::New(env);
    uint32_t index = 0;
    
    if (OpenClipboard(NULL)) {
        HDROP hDrop = HDROP(::GetClipboardData(CF_HDROP));
        if (hDrop != NULL) {
            UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
            for (UINT i = 0; i < fileCount; ++i) {
                WCHAR filePath[MAX_PATH + 1] = { 0 };
                DragQueryFileW(hDrop, i, filePath, MAX_PATH);
                std::string utf8Path = WideToUtf8(filePath);
                result.Set(index++, Napi::String::New(env, utf8Path));
            }
        }
        CloseClipboard();
    }
    
    return result;
}

void WriteFileNames(Napi::Env env, Napi::Array files) {
    std::vector<std::wstring> filePaths;
    uint32_t length = files.Length();
    
    for (uint32_t i = 0; i < length; i++) {
        Napi::Value value = files[i];
        if (value.IsString()) {
            std::string utf8Path = value.As<Napi::String>();
            filePaths.push_back(Utf8ToWide(utf8Path));
        }
    }
    
    if (filePaths.empty()) {
        return;
    }
    
    size_t totalSize = sizeof(DROPFILES);
    for (const auto& path : filePaths) {
        totalSize += (path.length() + 1) * sizeof(wchar_t);
    }
    totalSize += sizeof(wchar_t);
    
    HANDLE hGlobal = GlobalAlloc(GMEM_MOVEABLE, totalSize);
    if (!hGlobal) {
        return;
    }
    
    DROPFILES* df = (DROPFILES*)GlobalLock(hGlobal);
    if (!df) {
        GlobalFree(hGlobal);
        return;
    }
    
    df->pFiles = sizeof(DROPFILES);
    df->fWide = TRUE;
    
    wchar_t* fileList = (wchar_t*)(((BYTE*)df) + sizeof(DROPFILES));
    size_t offset = 0;
    
    for (const auto& path : filePaths) {
        memcpy(fileList + offset, path.c_str(), (path.length() + 1) * sizeof(wchar_t));
        offset += path.length() + 1;
    }
    fileList[offset] = L'\0';
    
    GlobalUnlock(hGlobal);
    
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_HDROP, hGlobal);
        CloseClipboard();
    } else {
        GlobalFree(hGlobal);
    }
}
