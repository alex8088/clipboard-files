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
				fileNames->Set(nIndex, String::NewFromUtf8(isolate, GBK2Utf8(szFilePathName), NewStringType::kNormal).ToLocalChecked());
			}
		}
		CloseClipboard(); // close clipboard
		
	}
	return fileNames;
}
