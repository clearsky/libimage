#pragma once
#include "LibImageBase.h"
#include "WGdi.h"
class LibImage:public LibImageBase
{
public:
	LibImage(bool init=false);
	~LibImage();
public:
	long Bind(HWND hwnd);

	long Capture(long x1, long y1, long x2, long y2, const std::wstring& path);
	long CapturePre(const std::wstring& path);

	long UseDict(int idx);
	long AddDict(int idx, const std::wstring& dict_str);
	long Ocr(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, std::wstring& ret);
	long OcrOne(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, std::wstring& ret);
	long OcrEx(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, std::map<point_t, std::wstring>& ret);
	long FindStrFast(long x1, long y1, long x2, long y2, const std::wstring& words, const std::vector<UINT64>& dfcolor_pre, double sim);
	long FindStrFastEx(long x1, long y1, long x2, long y2, const std::wstring& words, const std::vector<UINT64>& dfcolor_pre, double sim, std::map<point_t, std::wstring>& ret);
	long LoadPic(const std::wstring& name, WImage* pImg);
	long LoadPic(const std::wstring& name, const std::wstring& file_path);
	long LoadPic(const std::wstring& name, PBYTE data, long len);
	long FindPic(long x1, long y1, long x2, long y2, const std::vector<std::wstring>& names, UINT32 dfcolor_pre, double sim, point_desc_t& ret);
	long FindPicEx(long x1, long y1, long x2, long y2, const std::vector<std::wstring>& names, UINT32 dfcolor_pre, double sim, vpoint_desc_t& vpd);
private:
	WGdi wgdi;
};

