#include "WGdi.h"

WGdi::WGdi():_hwnd(0),_hmdc(nullptr), _hdc(nullptr),
_width(0), _height(0), _client_x(0), _client_y(0)
{
}

WGdi::~WGdi()
{
	if (_bind_state) {
		bind_clear();
	}
}

long WGdi::do_bind(HWND hwnd)
{
	if (!::IsWindow(hwnd)) return 0;
	_hwnd = hwnd;
	RECT rc;
	RECT rc2;
	::GetWindowRect(_hwnd, &rc);
	::GetClientRect(_hwnd, &rc2);

	_width = rc2.right - rc2.left;
	_height = rc2.bottom - rc2.top;
	POINT pt = { 0 };
	::ClientToScreen(_hwnd, &pt);
	dx_ = pt.x - rc.left;
	dy_ = pt.y - rc.top;

	_hdc = ::GetDC(NULL);
	if (!_hdc)  return 0;
	_hmdc = CreateCompatibleDC(_hdc);
	if (!_hmdc) return -2;
	return 1;
}

long WGdi::bind_clear()
{
	_hbmp_window = (HBITMAP)SelectObject(_hmdc, _hbmp_old);
	if (_hdc) DeleteDC(_hdc);
    _hdc = nullptr;
	if (_hmdc) DeleteDC(_hmdc);
	_hmdc = nullptr;
	if (_hbmp_window) DeleteObject(_hbmp_window);
	_hbmp_window = nullptr;
	return 1;
}

long WGdi::Bind(HWND hwnd)
{
	long ret = do_bind(hwnd);
	if (ret == 1) {
		_bind_state = 1;
	}
	else {
		UnBind();
	}
	
	return ret;
}

long WGdi::UnBind()
{
	if (_bind_state) {
		bind_clear();
	}
	_hwnd = 0;
	_bind_state = 0;
	_width = 0;
	_height = 0;
	_client_x = 0;
	_client_y = 0;
	dx_ = 0;
	dy_ = 0;
	_bfh = {};
	_bih = {};
	return 1;
}

long WGdi::RequestCapture(int x1, int y1, int w, int h, WImage& img)
{
	if (!::IsWindow(_hwnd)) return 0;
	
	_hbmp_window = CreateCompatibleBitmap(_hdc, w, h);
	_hbmp_old = (HBITMAP)SelectObject(_hmdc, _hbmp_window);
	// НиЭМ
	RECT rc;
	::GetWindowRect(_hwnd, &rc);
	int src_x = x1 + rc.left + dx_;
	int src_y = y1 + rc.top + dy_;

	_bih.biSize = sizeof(BITMAPINFOHEADER);
	_bih.biWidth = w;           
	_bih.biHeight = h;  
	_bih.biPlanes = 1;
	_bih.biBitCount = 32; 
	_bih.biCompression = BI_RGB;
	_bih.biSizeImage = ((w * _bih.biBitCount + 31) / 32) * 4 * h;

	_bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	_bfh.bfSize = _bfh.bfOffBits + _bih.biSizeImage;
	_bfh.bfType = static_cast<WORD>(0x4d42);

	if (!BitBlt(_hmdc, 0, 0, w, h, _hdc, src_x, src_y, SRCCOPY)) return 0;
	img.create(w, h, _bih.biBitCount >> 3);
	GetDIBits(_hdc, _hbmp_window, 0, h, img.getBytes(), (LPBITMAPINFO)&_bih, DIB_RGB_COLORS);
	
	return 1;
}


