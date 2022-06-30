#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <shlwapi.h>
#include <vector>
#include "LibImage_types.h"
#include "WImage.h"

namespace LibImage_help{

	//检查是否为透明图，返回透明像素个数, 四角颜色相同
	int check_transparent(WImage* img) {
		if (img->width() < 2 || img->height() < 2) return 0;  // 大小不合适 放弃
		UINT c0 = *img->begin(); // 拿到图片数据的开头，4字节
		bool x = c0 == img->at<UINT>(0, img->width() - 1) &&  // 右下角是否等于左下角
			c0 == img->at<UINT>(img->height() - 1, 0) && // 左上角是否等于左下角
			c0 == img->at<UINT>(img->height() - 1, img->width() - 1); // 左下角是否等于右上角
		if (!x) return 0;
		return 1;
	}

	// 获取非透明图部分
	void get_match_points(const WImage& img, std::vector<int>& points) {
		points.clear();
		UINT cbk = *img.begin();  // 图片开头像素点
		for (int i = 0; i < img.height(); ++i) {
			for (int j = 0; j < img.width(); ++j)  // 存储和开头点不同的像素索引
				if (cbk ^ img.at<UINT>(i, j)) points.push_back(i*img.width() + 1);
		}
	}

	int getOneNumber(UINT8 num) {
		int count = 0;
		while (num > 0) {
			num = num & (num - 1);
			count++;
		}
		return count;
	}
	std::wstring _s2wstring(const std::string& s) {
		size_t nlen = s.length();

		wchar_t* m_char;
		int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), nlen, NULL, 0);
		m_char = new wchar_t[len + 1];
		MultiByteToWideChar(CP_ACP, 0, s.data(), nlen, m_char, len);
		m_char[len] = '\0';
		std::wstring ws(m_char);
		delete[] m_char;
		return ws;
	}

	std::string _ws2string(const std::wstring& ws) {
		// std::string strLocale = setlocale(LC_ALL, "");
		// const wchar_t* wchSrc = ws.c_str();
		// size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
		// char *chDest = new char[nDestSize];
		// memset(chDest, 0, nDestSize);
		// wcstombs(chDest, wchSrc, nDestSize);
		// std::string strResult = chDest;
		// delete[]chDest;
		// setlocale(LC_ALL, strLocale.c_str());
		//return strResult;
		int nlen = ws.length();

		char* m_char;
		int len = WideCharToMultiByte(CP_ACP, 0, ws.data(), nlen, NULL, 0, NULL, NULL);
		m_char = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, ws.data(), nlen, m_char, len, NULL, NULL);
		m_char[len] = '\0';
		std::string s(m_char);
		delete[] m_char;
		return s;
	}

	long Path2GlobalPath(const std::wstring& file, const std::wstring& curr_path, std::wstring& out) {
		if (::PathFileExistsW(file.c_str())) {
			out = file;
			return 1;
		}
		out.clear();
		out = curr_path + L"\\" + file;
		if (::PathFileExistsW(out.c_str())) {
			return 1;
		}
		return 0;
	}



	void split(const std::wstring& s, std::vector<std::wstring>& v, const std::wstring& c)
	{
		std::wstring::size_type pos1, pos2;
		size_t len = s.length();
		pos2 = s.find(c);
		pos1 = 0;
		v.clear();
		while (std::wstring::npos != pos2)
		{
			v.emplace_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != len)
			v.emplace_back(s.substr(pos1));
	}

	void split(const std::string& s, std::vector<std::string>& v, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		size_t len = s.length();
		pos2 = s.find(c);
		pos1 = 0;
		v.clear();
		while (std::string::npos != pos2)
		{
			v.emplace_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != len)
			v.emplace_back(s.substr(pos1));
	}

	void wstring2upper(std::wstring& s) {
		std::transform(s.begin(), s.end(), s.begin(), towupper);
	}

	void string2upper(std::string& s) {
		std::transform(s.begin(), s.end(), s.begin(), toupper);
	}

	void wstring2lower(std::wstring& s) {
		std::transform(s.begin(), s.end(), s.begin(), towlower);
	}

	void string2lower(std::string& s) {
		std::transform(s.begin(), s.end(), s.begin(), tolower);
	}

	void replacea(std::string& str, const std::string& oldval, const std::string& newval) {
		size_t x0 = 0, dx = newval.length() - oldval.length() + 1;
		size_t idx = str.find(oldval, x0);
		while (idx != -1 && x0 >= 0) {
			str.replace(idx, oldval.length(), newval);
			x0 = idx + dx;
			idx = str.find(oldval, x0);
		}
	}

	void replacew(std::wstring& str, const std::wstring& oldval, const std::wstring& newval) {
		size_t x0 = 0, dx = newval.length() - oldval.length() + 1;
		size_t idx = str.find(oldval, x0);
		while (idx != -1 && x0 >= 0) {
			str.replace(idx, oldval.length(), newval);
			x0 = idx + dx;
			idx = str.find(oldval, x0);
		}
	}

	std::ostream& operator<<(std::ostream& o, point_t const& rhs) {
		o << rhs.x << "," << rhs.y;
		return o;
	}

	std::wostream& operator<<(std::wostream& o, point_t const& rhs) {
		o << rhs.x << L"," << rhs.y;
		return o;
	}

	std::ostream& operator<<(std::ostream& o, FrameInfo const& rhs) {
		o << "hwnd:" << rhs.hwnd << std::endl
			<< "frameId:" << rhs.frameId << std::endl
			<< "time:" << rhs.time << std::endl
			<< "height" << rhs.height << std::endl
			<< "width:" << rhs.width << std::endl;
		return o;
	}
	std::wostream& operator<<(std::wostream& o, FrameInfo const& rhs) {
		o << L"hwnd:" << rhs.hwnd << std::endl
			<< L"frameId:" << rhs.frameId << std::endl
			<< L"time:" << rhs.time << std::endl
			<< L"height" << rhs.height << std::endl
			<< L"width:" << rhs.width << std::endl;
		return o;
	}

}