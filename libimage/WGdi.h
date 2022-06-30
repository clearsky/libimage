#pragma once
#include <windows.h>
#include "WImage.h"
class WGdi
{
public:
	WGdi();
	~WGdi();
public:
	long Bind(HWND hwnd);
	long UnBind();
	long RequestCapture(int x1, int y1, int w, int h, WImage& img);
	bool CheckBind() { return _bind_state; }
private:
	long bind_clear();
	long do_bind(HWND hwnd);
private:
	HWND _hwnd;

	HDC _hmdc;
	HDC _hdc;
	//位图句柄
	HBITMAP _hbmp_window = NULL;
	HBITMAP _hbmp_old = NULL;
	//bmp 文件头
	BITMAPFILEHEADER _bfh = { 0 };
	BITMAPINFOHEADER _bih = { 0 };//位图信息头

	long _width; // 客户区宽度
	long _height; // 客户区高度
	long _client_x; 
	long _client_y;
	int dx_, dy_; //  客户区左上角与边框左上角偏移

	long _bind_state;
};

