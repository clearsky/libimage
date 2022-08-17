#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include "./libimage/LibImage_types.h"
#include "./libimage/WImage.h"
#include "./libimage/color.h"
#include "./libimage/dict.h"
#include "..\libimage\ThreadPool.h"
class LibImageBase
{
public:
	LibImageBase();
	~LibImageBase();
public:
	long Capture(const std::wstring& file);
	long UseDict(int idx);
	long Ocr(Dict& dict, double sim, std::wstring& ret_str);
	long OcrOne(Dict& dict, double sim, std::wstring& ret_str);
	long OcrOneWithPos(Dict& dict, double sim, ocr_ret_with_pos& ret_str);
	long OcrEx(Dict& dict, double sim, std::map<point_t, std::wstring>& ret);
	long FindStrFast(const std::wstring& str, double sim);
	long FindStrFastEx(const std::wstring& str,  double sim, std::map<point_t, std::wstring>& ret);
	long AddDict(int index, const std::wstring& str);

	long LoadPic(const std::wstring& name, WImage* pImg);
	long FindPic(std::vector<WImage*>& pics, color_t dfcolor, double sim, point_desc_t&ret);
	long FindPicEx(std::vector<WImage*>& pics, color_t dfcolor, double sim, vpoint_desc_t& vpd);
	long FindColor(std::vector<color_df_t>& colors, point_t& ret, int sim_color);
	long CmpColor(color_t color, std::vector<color_df_t>& colors, double sim);
protected:
	void record_sum(const WImageBin& gray);
	int region_sum(int x1, int y1, int x2, int y2);
	template<typename T>
	void _bin_ocr(const T& words, double sim, std::map<point_t, std::wstring>& ps, bool find_one = false);
	void _find_pic(WImage* pic, const color_t& dfcolor,  long&x, long&y);
	void _find_pic_ex(WImage* pic, const color_t& dfcolor, std::vector<point_t>& ret);
public:
	void bgr2binary(std::vector<color_df_t>& colors);
	void bin_ocr(const Dict& dict, double sim, std::map<point_t, std::wstring>& ps, bool one =false);
	void bin_find_str(const std::wstring& strs, double sim, std::map<point_t, std::wstring>& ps, bool find_one = false);
public:
	int VUINT642Colordfs(const std::vector<UINT64>& dfcolor_pre, std::vector<color_df_t>& colors);
	void UINT322Color(const UINT32 v, color_t& color);
	bool word_part_match(const rect_t& rc, int max_error, const uint8_t* data);
	void SetOffset(int x1, int y1) {
		_x1 = x1;
		_y1 = y1;
	}
protected:
	const static int _max_dict = 20;
#if LIBIMAGE_UNIT_DEBUG
public:
#else:
protected:
#endif
	Dict _dicts[_max_dict];
	std::map<std::wstring, std::vector<int>>  _words_cache[_max_dict];
	std::map<std::wstring, WImage*> _img_cache;
	int _cur_dic_idx;
	ThreadPool m_threadPool;
#if LIBIMAGE_UNIT_DEBUG
public:
#else:
protected:
#endif
	WImage _src; // 源图像
	WImageBin _record; // 记录图
	WImageBin _binary; // 二值化图像
	WImage _sum; // 像素密度图
	int _x1 = 0;
	int _y1 = 0;
};

