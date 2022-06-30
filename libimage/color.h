#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>
#include <math.h>
#define WORD_BKCOLOR 0
#define WORD_COLOR 1

#define color2uint(color) (*(uint*)&color)

template<typename T>
constexpr T W_ABS(T x) {
	return x < 0 ? -x : x;
}
template<typename T>
constexpr bool IN_RANGE(T lhs, T rhs, T df) {
	return abs(lhs.b - rhs.b) <= df.b
		&& abs(lhs.g - rhs.g) <= df.g
		&& abs(lhs.r - rhs.r) <= df.r;
}

#pragma pack(1)
// rgb颜色和其内的各项操作
struct color_t {
	//b is in low address ,alpha is in high address
	BYTE b, g, r, alpha;
	color_t() :b(0), g(0), r(0), alpha(0) {}
	color_t(int b_, int g_, int r_) :b(b_), g(g_), r(r_), alpha(0xffu) {}

	color_t& str2color(const std::wstring& s) {
		int r = 0, g = 0, b = 0;
		std::wstring ss = s;
		std::transform(ss.begin(), ss.end(), ss.begin(), ::towupper);
		int cnt = swscanf(ss.c_str(), L"%02X%02X%02X", &r, &g, &b);
		this->b = b; this->r = r; this->g = g;
		return *this;
	}
	color_t& str2color(const std::string& s) {
		int r, g, b;
		std::string ss = s;
		std::transform(ss.begin(), ss.end(), ss.begin(), ::toupper);
		int cnt = sscanf(ss.c_str(), "%02X%02X%02X", &r, &g, &b);
		this->b = b; this->r = r; this->g = g;
		return *this;
	}
	std::string tostr() {
		char buff[10];
		sprintf(buff, "%02X%02X%02X", r, g, b);
		return buff;
	}
	std::wstring towstr() {
		wchar_t buff[10];
		wsprintf(buff, L"%02X%02X%02X", r, g, b);
		return buff;
	}
	BYTE toGray() const {
		return (r * 299 + g * 587 + b * 114 + 500) / 1000;
	}
	
};
#pragma pack()
// 颜色和偏色,两个color_t
struct color_df_t {
	color_t color;
	color_t df;
	bool compare(const color_t& color_) const {
		return ((abs(color.r - color_.r) <= df.r) &&
			(abs(color.g - color_.g) <= df.g) &&
			(abs(color.b - color_.b) <= df.b));
	}
};
// 在一个点有多个颜色和偏色
struct pt_cr_df_t {
	int x, y;
	std::vector<color_df_t> crdfs;
};