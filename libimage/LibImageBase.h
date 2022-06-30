#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include "LibImage_types.h"
#include "WImage.h"
#include "color.h"
#include "dict.h"
#include "ThreadPool.h"
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
	long OcrEx(Dict& dict, double sim, std::map<point_t, std::wstring>& ret);
	long FindStrFast(const std::wstring& str, double sim);
	long FindStrFastEx(const std::wstring& str,  double sim, std::map<point_t, std::wstring>& ret);
	long AddDict(int index, const std::wstring& str);

	long LoadPic(const std::wstring& name, WImage* pImg);
	long FindPic(std::vector<WImage*>& pics, color_t dfcolor, double sim, point_desc_t&ret);
	long FindPicEx(std::vector<WImage*>& pics, color_t dfcolor, double sim, vpoint_desc_t& vpd);
protected:
	void record_sum(const WImageBin& gray);
	int region_sum(int x1, int y1, int x2, int y2);
	template<typename T>
	void _bin_ocr(const T& words, double sim, std::map<point_t, std::wstring>& ps, bool find_one = false);
	void _find_pic(WImage* pic, const color_t& dfcolor,  long&x, long&y);
	void _find_pic_ex(WImage* pic, const color_t& dfcolor, std::vector<point_t>& ret);
protected:
	void bgr2binary(std::vector<color_df_t>& colors);
	void bin_ocr(const Dict& dict, double sim, std::map<point_t, std::wstring>& ps, bool one =false);
	void bin_find_str(const std::wstring& strs, double sim, std::map<point_t, std::wstring>& ps, bool find_one = false);
protected:
	int VUINT642Colordfs(const std::vector<UINT64>& dfcolor_pre, std::vector<color_df_t>& colors);
	void UINT322Color(const UINT32 v, color_t& color);
	bool word_part_match(const rect_t& rc, int max_error, const uint8_t* data);
protected:
	const static int _max_dict = 20;
protected:
	Dict _dicts[_max_dict];
	std::map<std::wstring, std::vector<int>>  _words_cache[_max_dict];
	std::map<std::wstring, WImage*> _img_cache;
	int _cur_dic_idx;
	ThreadPool m_threadPool;
protected:
	WImage _src; // Ô´Í¼Ïñ
	WImageBin _record; // ¼ÇÂ¼Í¼
	WImageBin _binary; // ¶þÖµ»¯Í¼Ïñ
	WImage _sum; // ÏñËØÃÜ¶ÈÍ¼
};

