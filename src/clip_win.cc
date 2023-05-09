#include "clip_win.h"

char *GBK2Utf8(const char *strGBK)
{
    WCHAR *str1;
    int n = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
    str1 = new WCHAR[n];
    MultiByteToWideChar(CP_ACP, 0, strGBK, -1, str1, n);
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char *str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    delete[] str1;
    str1 = NULL;
    return str2;
}

Napi::Value get_file_names(const Napi::CallbackInfo &info)
{

    Napi::Env env = info.Env();
    // uint32_t index = 0;

    Napi::Array fileNames;
    if (OpenClipboard(NULL)) // open clipboard
    {
        HDROP hDrop = HDROP(::GetClipboardData(CF_HDROP)); // get the file path hwnd of clipboard
        if (hDrop != NULL)
        {
            char szFilePathName[MAX_PATH + 1] = {0};

            UINT nNumOfFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); // get the count of files
            fileNames = Napi::Array::New(env, nNumOfFiles);
            for (UINT nIndex = 0; nIndex < nNumOfFiles; ++nIndex)
            {
                memset(szFilePathName, 0, MAX_PATH + 1);

                DragQueryFile(hDrop, nIndex, (LPSTR)szFilePathName, MAX_PATH); // get file name
                fileNames.Set(nIndex, Napi::String::New(env, GBK2Utf8(szFilePathName)));
            }
        }
        CloseClipboard(); // close clipboard
    }
    return fileNames;
}

std::wstring stringToWstring(const std::string &str)
{
    int nLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (nLen == 0)
        return std::wstring(L"");

    wchar_t *wide = new wchar_t[nLen];
    if (!wide)
        return std::wstring(L"");

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, nLen);
    std::wstring wstr(wide);
    delete[] wide;
    wide = NULL;
    return wstr;
}

Napi::Value write_file_names(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1)
    {
        Napi::Error::New(env, "Expected exactly one argument")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    if (!(info[0].IsArray() || info[0].IsString()))
    {
        Napi::Error::New(env, "Expect a string or array of strings")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    std::vector<wstring> files;
    if (info[0].IsArray())
    {
        Napi::Array fileNames = info[0].As<Napi::Array>();

        for (size_t i = 0; i < fileNames.Length(); i++)
        {
            Napi::Value item = fileNames.Get(i);
            if (!item.IsString())
            {
                Napi::Error::New(env, "Expect a string or array of strings")
                    .ThrowAsJavaScriptException();
                return info.Env().Undefined();
            }
            std::string filename = item.ToString();
            files.push_back(stringToWstring(filename));
        }
    }
    else
    {
        std::string filename = info[0].As<Napi::String>();
        files.push_back(stringToWstring(filename));
    }

    size_t bytes = sizeof(DROPFILES);
    for (size_t i = 0; i < files.size(); ++i)
        bytes += (files[i].length() + 1) * sizeof(wchar_t);
    bytes += sizeof(wchar_t);
    HANDLE hdata = ::GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hdata)
        return env.Undefined();
    DROPFILES *drop_files = static_cast<DROPFILES *>(::GlobalLock(hdata));
    drop_files->pFiles = sizeof(DROPFILES);
    drop_files->fWide = TRUE;
    BYTE *data = reinterpret_cast<BYTE *>(drop_files) + sizeof(DROPFILES);
    // Copy the strings stored in 'files' with proper NULL separation.
    wchar_t *data_pos = reinterpret_cast<wchar_t *>(data);
    for (size_t i = 0; i < files.size(); ++i)
    {
        size_t offset = files[i].length() + 1;
        memcpy(data_pos, files[i].c_str(), offset * sizeof(wchar_t));
        data_pos += offset;
    }
    data_pos[0] = L'\0'; // Double NULL termination after the last string.
    ::GlobalUnlock(hdata);
    if (OpenClipboard(NULL))
    {
        EmptyClipboard();
        SetClipboardData(CF_HDROP, hdata);
        CloseClipboard();
    }
    return env.Undefined();
}
