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

void write_file_names(Isolate *isolate, Local<Array> fileNames)
{
	Local<Context> context = isolate->GetCurrentContext();
	TCHAR * sFiles = NULL;
	int nLen = 0;
	for (size_t i = 0; i < fileNames->Length(); i++) {
		MaybeLocal<Value> maybeIndex = fileNames->Get(context, i);
		Local<Value> index = maybeIndex.ToLocalChecked();

		String::Utf8Value path(isolate, index);

		int size = nLen;
		nLen += path.length() + 1;
		sFiles = (TCHAR*)realloc(sFiles, nLen * sizeof(TCHAR));
#if UNICODE
		std::string narrow(*path);

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(narrow);

		std::wcscpy(sFiles + size, wide.c_str);
#else
		std::strcpy(sFiles + size, *path);
#endif
	}
	DROPFILES dobj = { 20, { 0, 0 }, 0, 1 };
#if UNICODE
	int nGblLen = sizeof(dobj) + nLen + 5;
#else
    int nGblLen = sizeof(dobj) + nLen * 2 + 5;
#endif
	HGLOBAL hGbl = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, nGblLen);
	TCHAR * sData = (char*)::GlobalLock(hGbl);
	memcpy(sData, &dobj, 20);
	TCHAR * sWStr = sData + 20;
#if UNICODE
	for (int i = 0; i < nLen; i += 1) {
		sWStr[i] = sFiles[i];
	}
#else
	for (int i = 0; i < nLen * 2; i += 2) {
		sWStr[i] = sFiles[i / 2];
	}
#endif
	::GlobalUnlock(hGbl);
    if (OpenClipboard(NULL)) {
		EmptyClipboard();
		SetClipboardData(CF_HDROP, hGbl);
		CloseClipboard();
	}
	free(sFiles);
}
