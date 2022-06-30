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
	//λͼ���
	HBITMAP _hbmp_window = NULL;
	HBITMAP _hbmp_old = NULL;
	//bmp �ļ�ͷ
	BITMAPFILEHEADER _bfh = { 0 };
	BITMAPINFOHEADER _bih = { 0 };//λͼ��Ϣͷ

	long _width; // �ͻ������
	long _height; // �ͻ����߶�
	long _client_x; 
	long _client_y;
	int dx_, dy_; //  �ͻ������Ͻ���߿����Ͻ�ƫ��

	long _bind_state;
};

