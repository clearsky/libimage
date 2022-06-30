#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "WImage.h"
#include "LibImage_types.h"
#include "helpfunc.h"

class WordInfo {
public:
	WordInfo() :w(0), h(0), bit_count(0){}
	~WordInfo() {}
public:
	UINT8 w;
	UINT8 h;
	UINT16 bit_count;
	wchar_t name[8] = {0};
};

class Word {
public:
	Word() {}
	~Word() {}
public:
	void set_name(const std::wstring& s) {
		size_t sLen = s.length() < 8 ? s.length() : 7;
		memcpy_s(info.name, sLen * sizeof(wchar_t), s.c_str(), sLen * sizeof(wchar_t));
		info.name[sLen] = L'\0';
	}
	void init() {
		data.resize((info.w * info.h + 7) / 8);
		std::fill(data.begin(), data.end(), 0);
	}
public:
	bool operator == (const Word& rhs) {
		if (info.w != rhs.info.w ||
			info.h != rhs.info.h ||
			info.bit_count != rhs.info.bit_count) {
			return false;
		}
		for (size_t i = 0; i < data.size(); ++i) {
			if (data[i] != rhs.data[i]) {
				return false;
			}
		}
		return true;
	}
	const Word& operator *()  const{
		return *this;
	}
public:
	WordInfo info;
	std::vector<UINT8> data; //size is (w*h+7)/8
};
struct dict_info_t {
	__int16 _this_ver;
	__int16 _word_count;
	__int32 _check_code;
	dict_info_t() :_this_ver(1), _word_count(0) { _check_code = _word_count ^ _this_ver; }
};
class Dict {
public:
public:
	Dict():_word_count(0) {}
	~Dict() {}
public:
	void clear() {
		_word_count = 0;
		_words.resize(0);
	}
	void sort_dict() {
		std::stable_sort(_words.begin(), _words.end(), 
			[](const Word& lhs, const Word& rhs) {
				int dh = lhs.info.h - rhs.info.h;
				int dw = lhs.info.w - rhs.info.w;
				return dh > 0 || (dh == 0 && dw > 0) ||
					(dh == 0 && dw == 0 && lhs.info.bit_count < rhs.info.bit_count);
			});
	}
	UINT8 reverse8(UINT8 c)
	{
		c = (c & 0x55) << 1 | (c & 0xAA) >> 1;
		c = (c & 0x33) << 2 | (c & 0xCC) >> 2;
		c = (c & 0x0F) << 4 | (c & 0xF0) >> 4;
		return c;
	}
	bool add_word_dm(const std::wstring& wstr) {
		if (wstr.empty()) return false;
		Word word;
		word.info.h = 11;
		
		std::vector<std::wstring> datas;
		LibImage_help::split(wstr, datas, L"$");
		if (datas.size() != 4) return false;

		std::wstring& bits_str = datas[0];
		std::wstring& name = datas[1];
		size_t bits_str_len = bits_str.length();
		size_t bits_len = (bits_str_len+2) / 2;
		word.data.resize(bits_len);

		int dm_bit_count = bits_str_len * 4;
		int width = (dm_bit_count) / 11;
		word.info.w = width;

		int bit_count = 0;
		UINT8 data1 = 0;
		UINT8 data2 = 0;
		for (size_t i = 0; i < bits_len;  ++i) {
			if (i * 2 +1 >= bits_str_len) {
				word.data[i] = 0;
				break;
			}
			if (bits_str[i*2] > L'9') {
				data1 = bits_str[i*2] - L'A'  + 10;
			}
			else {
				data1 = bits_str[i*2] - L'0';
			}
			data1 <<= 4;
			if (bits_str[i*2+1] > L'9') {
				data2 = bits_str[i*2+1] - L'A' + 10;
			}
			else {
				data2 = bits_str[i*2+1] - L'0';
			}
			data1 |= data2;
			data1 = reverse8(data1);
			bit_count += LibImage_help::getOneNumber(data1);
			word.data[i] = data1;
		}
		
		auto temp_data = word.data;
		std::fill(word.data.begin(), word.data.end(), 0);
		for (int i = 0; i < 11; ++i) {
			for (int j = 0; j < width; ++j) {
				int new_index = i * width + j;
				int old_index = j * 11 + 11 - i - 1;
				if (GET_BIT(temp_data.data()[old_index / 8], old_index & 7)) {
					SET_BIT(word.data.data()[new_index / 8], new_index & 7);
				}
			}
		}

		word.info.bit_count = bit_count;
		word.set_name(name);
		auto it = find(word);
		if (_words.empty() || it == _words.end()) {
			_words.push_back(word);
			_word_count = _words.size();
		}
		return true;
	}
	std::vector<Word>::iterator find(const Word& word) {
		for (auto it = _words.begin(); it != _words.end(); ++it) {
			if (*it == word)return it;
			return _words.end();
		}
	}
	void erase(const Word& word) {
		auto it = find(word);
		if (!_words.empty() && it != _words.end())
			_words.erase(it);
		_word_count = _words.size();
	}
	int size() const {
		return _word_count;
	}
	bool empty() const {
		return size() == 0;
	}
	const std::vector<Word>& getWords() const {
		return _words;
	}
private:
	UINT16 _word_count;
	std::vector<Word> _words;
};