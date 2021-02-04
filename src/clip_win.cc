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

Local<Array> get_file_names(Isolate *isolate)
{
	Local<Array> fileNames = Array::New(isolate, 0);
	Local<Context> context = isolate->GetCurrentContext();
	if (OpenClipboard(NULL)) // open clipboard
	{
		HDROP hDrop = HDROP(::GetClipboardData(CF_HDROP)); // get the file path hwnd of clipboard
		if (hDrop != NULL)
		{
			char szFilePathName[MAX_PATH + 1] = { 0 };
			UINT nNumOfFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); // get the count of files
			fileNames = Array::New(isolate, nNumOfFiles);
			for (UINT nIndex = 0; nIndex < nNumOfFiles; ++nIndex)
			{
				memset(szFilePathName, 0, MAX_PATH + 1);
				DragQueryFile(hDrop, nIndex, szFilePathName, MAX_PATH); // get file name
				fileNames->Set(context, nIndex, String::NewFromUtf8(isolate, GBK2Utf8(szFilePathName), NewStringType::kNormal).ToLocalChecked());
			}
		}
		CloseClipboard(); // close clipboard	
	}
	return fileNames;
}

std::wstring stringToWstring(const std::string& str)
{
    int nLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (nLen == 0) 
        return std::wstring(L"");

    wchar_t* wide = new wchar_t[nLen];
    if (!wide) 
        return std::wstring(L"");

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, nLen);
    std::wstring wstr(wide);
    delete[] wide;
    wide = NULL;
    return wstr;
}

void write_file_names(Isolate *isolate, Local<Array> fileNames)
{
	Local<Context> context = isolate->GetCurrentContext();

    std::vector<wstring> files;
    for (size_t i = 0; i < fileNames->Length(); i++) {
        MaybeLocal<Value> maybeIndex = fileNames->Get(context, i);
        Local<Value> index = maybeIndex.ToLocalChecked();
        String::Utf8Value path(isolate, index);
        std::string pathStr(*path);
        files.push_back(stringToWstring(pathStr));
    }

    size_t bytes = sizeof(DROPFILES);
    for (size_t i = 0; i < files.size(); ++i)
        bytes += (files[i].length() + 1) * sizeof(wchar_t);
    bytes += sizeof(wchar_t);
    HANDLE hdata = ::GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hdata)
       return;
    DROPFILES* drop_files = static_cast<DROPFILES*>(::GlobalLock(hdata));
    drop_files->pFiles = sizeof(DROPFILES);
    drop_files->fWide = TRUE;
    BYTE* data = reinterpret_cast<BYTE*>(drop_files) + sizeof(DROPFILES);
    // Copy the strings stored in 'files' with proper NULL separation.
    wchar_t* data_pos = reinterpret_cast<wchar_t*>(data);
    for (size_t i = 0; i < files.size(); ++i) {
        size_t offset = files[i].length() + 1;
        memcpy(data_pos, files[i].c_str(), offset * sizeof(wchar_t));
        data_pos += offset;
	}
    data_pos[0] = L'\0';  // Double NULL termination after the last string.
    ::GlobalUnlock(hdata);
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        SetClipboardData(CF_HDROP, hdata);
        CloseClipboard();
	}
}
