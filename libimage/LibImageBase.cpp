#include "./libimage/LibImage.h"

LibImage::LibImage(bool init)
{
	std::string strLocale = setlocale(LC_ALL, "");
	setlocale(LC_ALL, strLocale.c_str());
	if (1 || init) {
		//屏幕整体尺寸
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		_src.create(cx, cy, 3);
	}
	wgdi.Bind(::GetDesktopWindow());
}

LibImage::~LibImage()
{
}

long LibImage::Bind(HWND hwnd)
{
	return wgdi.Bind(hwnd);
}

long LibImage::Capture(long x1, long y1, long x2, long y2, const std::wstring& path)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	LibImageBase::Capture(path);
	return 1;
}

long LibImage::CapturePre(const std::wstring& path)
{
	if (_src.empty()) return 0;
	LibImageBase::Capture(path);
	return 1;
}

long LibImage::UseDict(int idx)
{
	return LibImageBase::UseDict(idx);
}

long LibImage::AddDict(int idx, const std::wstring& dict_str)
{
	return LibImageBase::AddDict(idx, dict_str);
}

long LibImage::Ocr(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, std::wstring& ret)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	bgr2binary(dfcolors);
	SetOffset(x1, y1);
	return LibImageBase::Ocr(_dicts[_cur_dic_idx], sim, ret);
}

long LibImage::OcrOne(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, std::wstring& ret)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	bgr2binary(dfcolors);
	SetOffset(x1, y1);
	return LibImageBase::OcrOne(_dicts[_cur_dic_idx], sim, ret);
}

long LibImage::OcrOneWithPos(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, ocr_ret_with_pos& ret)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	bgr2binary(dfcolors);
	SetOffset(x1, y1);
	return LibImageBase::OcrOneWithPos(_dicts[_cur_dic_idx], sim, ret);
}

long LibImage::OcrEx(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, std::map<point_t, std::wstring>& ret)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	bgr2binary(dfcolors);
	SetOffset(x1, y1);
	return LibImageBase::OcrEx(_dicts[_cur_dic_idx], sim, ret);
}

long LibImage::FindStrFast(long x1, long y1, long x2, long y2, const std::wstring& words, const std::vector<UINT64>& dfcolor_pre, double sim)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1-5, y1-5, x2 - x1 + 10, y2 - y1+10, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	bgr2binary(dfcolors);
	SetOffset(x1, y1);
	return LibImageBase::FindStrFast(words, sim);
}

long LibImage::FindStrFastEx(long x1, long y1, long x2, long y2, const std::wstring& words, const std::vector<UINT64>& dfcolor_pre, double sim, std::map<point_t, std::wstring>& ret)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	bgr2binary(dfcolors);
	SetOffset(x1, y1);
	return LibImageBase::FindStrFastEx(words, sim, ret);
}

long LibImage::LoadPic(const std::wstring& name, WImage* pImg)
{
	return LibImageBase::LoadPic(name, pImg);
}

long LibImage::LoadPic(const std::wstring& name, const std::wstring& file_path)
{
	WImage* img = new WImage;
	if (!img->read(file_path.c_str())) {
		delete img;
		return 0;
	}
	return LibImageBase::LoadPic(name, img);
}

long LibImage::LoadPic(const std::wstring& name, PBYTE data, long len)
{
	WImage* img = new WImage;
	if (!img->read(data, len) ){
		delete img;
		return 0;
	}
	return LibImageBase::LoadPic(name, img);
}

long LibImage::LoadPic(const std::wstring& name, int RES_ID)
{
	WImage* img = new WImage;
	if (!img->read(RES_ID)) {
		delete img;
		return 0;
	}
	return LibImageBase::LoadPic(name, img);
}

long LibImage::FindPic(long x1, long y1, long x2, long y2, const std::vector<std::wstring>& names, UINT32 dfcolor_pre, double sim, point_desc_t& ret)
{
	if (!wgdi.CheckBind()) return 0;
	if (!names.size()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	color_t color;
	UINT322Color(dfcolor_pre, color);
	std::vector<WImage*> imgs;
	for (auto&& item : names) {
		if (!_img_cache.count(item)) continue;
		imgs.push_back(_img_cache[item]);
	}
	SetOffset(x1, y1);
	return LibImageBase::FindPic(imgs, color, sim, ret);
}

long LibImage::FindPicEx(long x1, long y1, long x2, long y2, const std::vector<std::wstring>& names, UINT32 dfcolor_pre, double sim, vpoint_desc_t& vpd)
{
	if (!wgdi.CheckBind()) return 0;
	if (!names.size()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	color_t color;
	UINT322Color(dfcolor_pre, color);
	std::vector<WImage*> imgs;
	for (auto&& item : names) {
		if (!_img_cache.count(item)) continue;
		imgs.push_back(_img_cache[item]);
	}
	SetOffset(x1, y1);
	return LibImageBase::FindPicEx(imgs, color, sim, vpd);
}

long LibImage::FindColor(long x1, long y1, long x2, long y2, const std::vector<UINT64>& dfcolor_pre, double sim, point_t& ret)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(x1, y1, x2 - x1, y2 - y1, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	SetOffset(x1, y1);
	sim = 0.5 + sim / 2;
	int m_simColor = 9;
	if (sim >= 0.8 && sim < 0.9) m_simColor = 18;
	else if (sim >= 0.7 && sim < 0.8) m_simColor = 29;
	else if (sim >= 0.6 && sim < 0.7) m_simColor = 38;
	else if (sim >= 0.5 && sim < 0.6) m_simColor = 49;
	return LibImageBase::FindColor(dfcolors, ret, m_simColor);
}

long LibImage::CmpColor(long x1, long y1, const std::vector<UINT64>& dfcolor_pre, double sim)
{
	if (!wgdi.CheckBind()) return 0;
	wgdi.RequestCapture(0, 0, x1+1, y1+1, _src);
	std::vector<color_df_t> dfcolors;
	VUINT642Colordfs(dfcolor_pre, dfcolors);
	return LibImageBase::CmpColor(_src.at<color_t>(0,x1), dfcolors, sim);
}

