// cpp-screenshot.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
void gdiscreen();

void gdiscreen()
{
	using namespace Gdiplus;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		HDC scrdc, memdc;
		HBITMAP membit;
		HWND hwnd = GetActiveWindow();
		scrdc = ::GetDC(hwnd);
		//scrdc = ::GetDC(0);//Get device context to start getting instance of screen to do grabbing//Check address of scrdc is not 0
		assert( scrdc != NULL ); 
		int Height = GetSystemMetrics(SM_CYSCREEN);
		assert( Height != 0 ); 
		int Width = GetSystemMetrics(SM_CXSCREEN);
		assert( Width != 0 ); 
		memdc = CreateCompatibleDC(scrdc);
		assert( memdc != NULL ); 
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		assert( membit != NULL ); 
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
		assert( hOldBitmap != NULL ); 
		//transfer the screendata from position 0,0 to witdth/height from screen device context(scrdc) to memory device context (memdc)
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
		Gdiplus::Bitmap bitmap(membit, NULL);//create bitmap and initialized it witl null values
		assert( bitmap != NULL ); 
		CLSID clsid;//A CLSID is a globally unique identifier that identifies a COM class object
		GetEncoderClsid(L"image/jpeg", &clsid);
		bitmap.Save(L"C:\\temp2\\screen.jpeg", &clsid);
		SelectObject(memdc, hOldBitmap);
		DeleteObject(memdc);//clean up to avoid memory leak
		DeleteObject(membit);//clean up to avoid memory leak
		::ReleaseDC(0, scrdc);//release the device context else it will show weird stuff
	}
	GdiplusShutdown(gdiplusToken);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	using namespace Gdiplus;
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return 0;
}

int main()
{
	gdiscreen();
}
