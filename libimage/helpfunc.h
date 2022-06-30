#pragma once
#include "LibImage_types.h"
namespace LibImage_help {
	int check_transparent(WImage* img);
	void get_match_points(const WImage& img, std::vector<int>& points);
	int getOneNumber(UINT8 num);
	//#include "../background/display/frameInfo.h"
	std::wstring _s2wstring(const std::string& s);
	std::string _ws2string(const std::wstring& s);
	//将路径转化为全局路径
	long Path2GlobalPath(const std::wstring& file, const std::wstring& curr_path, std::wstring& out);

	void split(const std::wstring& s, std::vector<std::wstring>& v, const std::wstring& c);
	void split(const std::string& s, std::vector<std::string>& v, const std::string& c);

	void wstring2upper(std::wstring& s);
	void string2upper(std::string& s);

	void wstring2lower(std::wstring& s);
	void string2lower(std::string& s);

	void replacea(std::string& str, const std::string& oldval, const std::string& newval);
	void replacew(std::wstring& str, const std::wstring& oldval, const std::wstring& newval);

	int inline hex2bin(int c) {
		return c <= L'9' ? c - L'0' : c - L'A' + 10;
	};

	int inline bin2hex(int c) {
		int ans = 0;
		int c1 = c >> 4 & 0xf;
		int c2 = c & 0xf;
		ans |= (c1 <= 9 ? c1 + L'0' : c1 + 'A' - 10) << 8;
		ans |= c2 <= 9 ? c2 + L'0' : c2 + 'A' - 10;
		return ans;
	};

	constexpr int PTY(UINT pt) {
		return pt >> 16;
	}

	constexpr int PTX(UINT pt) {
		return pt & 0xffff;
	}

	template<typename T>
	void nextVal(const T& t, int* next) {
		next[0] = -1;
		int k = -1, j = 0;
		while (j < (int)t.size() - 1) {
			if (k == -1 || t[k] == t[j]) {
				k++;
				j++;
				next[j] = k;
			}
			else {
				k = next[k];
			}
		}
	}
	template<typename T>
	int kmp(const T& s, const T& t) {
		std::vector<int> next(t.size());
		nextVal(t, next.data());
		int i = 0, j = 0;
		while (i < (int)s.size() && j < (int)t.size()) {
			if (j == -1 || s[i] == t[j]) {
				i++;
				j++;
			}
			else {
				j = next[j];
			}
		}
		return j == s.size() ? i - j : -1;
	}

	std::ostream& operator<<(std::ostream& o, point_t const& rhs);
	std::wostream& operator<<(std::wostream& o, point_t const& rhs);

	std::ostream& operator<<(std::ostream& o, FrameInfo const& rhs);
	std::wostream& operator<<(std::wostream& o, FrameInfo const& rhs);
	
}
