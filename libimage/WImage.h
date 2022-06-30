#pragma once
#include <malloc.h>
#include <atlimage.h>
#include <windows.h>
#include <fstream>
#include <atlstr.h>
#include <vector>
#include <iostream>
#include "libimage_types.h"
#define DEEP 4
class WBinMatrix;

class WImage {
public:
	using iterator = unsigned __int32*;
	WImage() :_width(0),_height(0),_lineBits(0),_biBitCount(0),_pBits(nullptr){};
	WImage(int w, int h, int biBitCount) {
		if (!create(w, h, biBitCount)) {
			release();
		}
	}
	WImage(const WImage& rhs) {
		*this = rhs;
	}
	WImage(WImage&& rhs)  noexcept{
		*this = rhs;
	}
	~WImage() {
		release();
	};
public:
	void printData() {
		if (empty()) {
			std::cout << "empty image" << std::endl;
		}
		printf("ImageInfo:%dx%d,biBitCount:%d\n", _width, _height, _biBitCount << 3);
		for (int i = 0; i < _height; ++i) {
			for (int j = 0; j < _width; ++j) {
				printf("%8X ", at<UINT32>(i, j));
			}
			std::cout << std::endl;
		}
	}
	bool create(int w, int h, int biBitCount) {
		if (w <= 0 || h <= 0) {
			release();
			return false;
		}
		int size = w * h * DEEP;
		if (!_pBits) {
			_pBits = (unsigned char*)malloc(size);
		}
		else{
			if (size > bitSize()) {
				_pBits = (unsigned char*)realloc(_pBits, size);
			}
		}
		if (_pBits == nullptr) {
			release();
			return false;
		}
		_width = w;
		_height = h;
		setBiBitCount(biBitCount);
		return true;
	}
	void release() {
		_width = 0;
		_height = 0;
		_lineBits = 0;
		_biBitCount = 0;
		if (_pBits) {
			free(_pBits);
		}
		_pBits = nullptr;
	}
	int size() const  {
		return _width * _height;
	}
	int bitSize() const {
		return _lineBits *  _height;
	}
	bool empty() const {
		return !_width  || !_height || !_lineBits || !_biBitCount || !_pBits;
	}
	bool read(LPCTSTR file_path) {
		std::ifstream in;
		in.open(file_path, std::ios::in|std::ios::binary);
		if (!in) {
			release();
			return false;
		}
		BITMAPFILEHEADER bitmapFileHeader;
		in.read((char*)&bitmapFileHeader, sizeof(BITMAPFILEHEADER));
		if (bitmapFileHeader.bfType != 0x4d42) {
			in.close();
			release();
			return false;
		}
		BITMAPINFOHEADER bitmapInfoHeader;
		in.read((char*)&bitmapInfoHeader, sizeof(BITMAPINFOHEADER));
		in.seekg(sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER), std::ios::beg);
		if (create(bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, bitmapInfoHeader.biBitCount >> 3)) {
			std::pair<int, int> pitchInfo = getPitchInfo();
			PBYTE temp_data = new BYTE[bitmapInfoHeader.biSizeImage];
			in.read((char*)temp_data, bitmapInfoHeader.biSizeImage);
			translate(temp_data, _biBitCount, std::get<0>(pitchInfo));
			delete[] temp_data;
			in.close();
		}
		else {
			in.close();
			release();
			return false;
		}
		return true;
	}
	bool read(PBYTE memSrc, long len) {
		BITMAPFILEHEADER bitmapFileHeader;
		memcpy_s(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), memSrc, sizeof(BITMAPFILEHEADER));
		if (bitmapFileHeader.bfType != 0x4d42) {
			release();
			return false;
		}
		BITMAPINFOHEADER bitMapInfoHeader;
		memcpy_s(&bitMapInfoHeader, sizeof(BITMAPINFOHEADER), memSrc + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
		
		if (create(bitMapInfoHeader.biWidth, bitMapInfoHeader.biHeight, bitMapInfoHeader.biBitCount)) {
			std::pair<int, int> pitchInfo = getPitchInfo();
			PBYTE temp_p = memSrc + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
			PBYTE temp_data = new BYTE[bitMapInfoHeader.biSizeImage];
			memcpy_s(temp_data, bitMapInfoHeader.biSizeImage, temp_p, bitMapInfoHeader.biSizeImage);
			translate(temp_data, _biBitCount, std::get<0>(pitchInfo));
			delete[] temp_data;
		}
		else {
			release();
			return false;
		}
		return true;
	}
	bool read(ATL::CImage* img) {
		translate((unsigned char*)img->GetBits(), img->GetBPP() / 8, img->GetPitch());
		return true;
	}
	bool write(LPCTSTR file_path) const   {
		if (empty()) {
			return false;
		}
		std::pair<int, int> pitchInfo = getPitchInfo();
		int totalSize = bitSize() + std::get<1>(pitchInfo) * _height;
		BITMAPFILEHEADER bitmapFileHeader;
		bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
		bitmapFileHeader.bfType = 0x4d42;
		bitmapFileHeader.bfReserved1 = 0;
		bitmapFileHeader.bfReserved2 = 0;
		bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + totalSize;
		BITMAPINFOHEADER bitmapInfoHeader;
		bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfoHeader.biWidth = _width;
		bitmapInfoHeader.biHeight = _height;
		bitmapInfoHeader.biPlanes = 1;
		bitmapInfoHeader.biBitCount = _biBitCount << 3;
		bitmapInfoHeader.biCompression = 0;
		bitmapInfoHeader.biSizeImage = totalSize;
		bitmapInfoHeader.biXPelsPerMeter = 0;
		bitmapInfoHeader.biYPelsPerMeter = 0;
		bitmapInfoHeader.biClrUsed = 0;
		bitmapInfoHeader.biClrImportant = 0;
		std::ofstream out;
		out.open(file_path, std::ios::out | std::ios::binary);
		if (!out) {
			out.close();
			return false;
		}
		out.write((char*)&bitmapFileHeader, sizeof(BITMAPFILEHEADER));
		out.write((char*)&bitmapInfoHeader, sizeof(BITMAPINFOHEADER));

		PBYTE temp_data = new BYTE[totalSize];
		PBYTE temp_data_start = temp_data;
		PBYTE temp_p = _pBits;
		for (int i = 0; i < _height; ++i) {
			for (int j = 0; j < _width; ++j) {
				memcpy_s(temp_data, _biBitCount, temp_p, _biBitCount);
				temp_data += _biBitCount;
				temp_p += DEEP;
			}
			if (std::get<1>(pitchInfo)) {
				memset(temp_data, 0, std::get<1>(pitchInfo));
				temp_data += std::get<1>(pitchInfo);
			}
		}
		out.write((char*)temp_data_start, totalSize);
		out.close();
		delete temp_data_start;
		return true;
	}
	bool write24(LPCTSTR file_path) const {
		if (_biBitCount == 3) {
			return write(file_path);
		}
		WImage temp;
		temp.create(_width, _height, 3);
		temp.translate(_pBits, _biBitCount, std::get<0>(getPitchInfo()));
		temp.write(file_path);
		return true;
	}
	void translate(unsigned char* psrc, int pixSize, int pitch, bool bin = false) {
		auto pdst = _pBits;
		//gray
		if (pixSize == 1) {
			for (int i = 0; i < _height; ++i) {
				if (bin) {
					for (int j = 0; j < _width; ++j) {
						*pdst++ = psrc[j]?255:0;
						*pdst++ = psrc[j]?255:0;
						*pdst++ = psrc[j]?255:0;
						*pdst++ = 0xff;
					}
				}
				else {
					for (int j = 0; j < _width; ++j) {
						*pdst++ = psrc[j];
						*pdst++ = psrc[j];
						*pdst++ = psrc[j];
						*pdst++ = 0xff;
					}
				}
				
				psrc += pitch;
			}
		}//bgr
		else if (pixSize == 3) {
			for (int i = 0; i < _height; ++i) {
				for (int j = 0; j < _width; ++j) {
					*pdst++ = psrc[j * 3 + 0];
					*pdst++ = psrc[j * 3 + 1];
					*pdst++ = psrc[j * 3 + 2];
					*pdst++ = 0xff;
				}
				psrc += pitch;
			}
		}
		else if (pixSize == 4) {
			for (int i = 0; i < _height; ++i) {
				for (int j = 0; j < _width; ++j) {
					*pdst++ = psrc[j * 4 + 0];
					*pdst++ = psrc[j * 4 + 1];
					*pdst++ = psrc[j * 4 + 2];
					*pdst++ = psrc[j * 4 + 3];
				}
				psrc += pitch;
			}
		}
	}
	void fill(unsigned int val) {
		std::fill(begin(), end(), val);
	}
	void fill(int row, int col, int h, int w, unsigned int val) {
		for (int i = 0; i < h; ++i) {
			auto p = ptr<unsigned __int32>(row + i) + col;
			std::fill(p, p + w, val);
		}
	}
	std::pair<int, int> getPitchInfo() const {
		int widbitCount = _width * (_biBitCount * 8);
		int need_num = _width * _biBitCount;
		int pitch = ((widbitCount + 31) / 32 * 4);
		int del = pitch - need_num;
		return std::make_pair(pitch, del);
	}
public:
	template<typename T>
	T at(int y, int x) const {
		return ((T*)_pBits)[y * _width + x];
	}
	template<typename T>
	T& at(int y, int x)  {
		return((T*)_pBits)[y * _width + x];
	}
	template<typename T>
	T* ptr(int y) {
		return (T*)(_pBits + y * _width * DEEP);
	}
	template<typename T>
	const T* ptr(int y)const {
		return (T*)(_pBits + y * _width * DEEP);
	}
public:
	iterator begin() {
		return (iterator)_pBits;
	}
	iterator end() {
		return (iterator)_pBits + (size_t)_width * _height;
	}
	iterator begin() const {
		return (iterator)_pBits;
	}
	iterator end() const {
		return (iterator)_pBits + (size_t)_width * _height;
	}
public:
	int getBiBitCount()  const{
		return _biBitCount;
	}
	int getLineBits() const {
		return _lineBits;
	}
	int height() const {
		return _height;
	}
	int width() const {
		return _width;
	}
	PBYTE getBytes() const {
		return _pBits;
	}
public:
	WImage& operator=(const WImage& rhs) {
		if (rhs.empty()) {
			this->release();
		}
		else {
			if (this->create(rhs._width, rhs._height, rhs._biBitCount)) {
				int size = _height* _width * DEEP;
				memcpy_s(this->_pBits, size, rhs._pBits, size);
			}
			else {
				release();
			}
		}
		return *this;
	 }
	WImage& operator = (WImage&& rhs) noexcept {
		if (rhs.empty()) {
			this->release();
			return *this;
		}
		this->_height = rhs._height;
		this->_width = rhs._width;
		this->_biBitCount = rhs._biBitCount;
		this->_lineBits = rhs._lineBits;
		this->_pBits = rhs._pBits;
		rhs._pBits = nullptr;
		rhs.release();
		return *this;
	}
private:
	void setBiBitCount(int bc) {
		_biBitCount = bc;
		_lineBits = _width * _biBitCount;
	}
private:
	int _biBitCount; // 图片位数
	int _lineBits; // 每行字节数
	int _width;  // 图片宽度
	int _height; // 图片高度
	PBYTE _pBits; // 位图数据
};

//单通道图像
class  WImageBin {
public:
	using iterator = unsigned char*;
	WImageBin() :_width(0), _height(0) {}
	WImageBin(const WImageBin& rhs) {
		this->_width = rhs._width;
		this->_height = rhs._height;
		this->_pixels = rhs._pixels;
	}
	WImageBin& operator=(const WImageBin& rhs) {
		this->_width = rhs._width;
		this->_height = rhs._height;
		this->_pixels = rhs._pixels;
		return *this;
	}
public:
	void create(int w, int h) {
		_width = w, _height = h;
		_pixels.resize(w * (size_t)h);
	}
	void release() {
		clear();
		std::vector <BYTE>().swap(_pixels);
	}
	void clear() {
		_width = _height = 0;
	}
	int size()const {
		return _width * _height;
	}
	bool empty()const {
		return _width == 0;
	}
	PBYTE data() {
		return _pixels.data();
	}
	BYTE at(int y, int x)const {
		return _pixels[y * (size_t)_width + x];
	}
	BYTE& at(int y, int x) {
		return _pixels[y * (size_t)_width + x];
	}
	PBYTE ptr(int y) {
		return _pixels.data() + y * (size_t)_width;
	}
	BYTE const* ptr(int y) const {
		return _pixels.data() + y * (size_t)_width;
	}
	void fromWImage(const WImage& img4) {
		create(img4.width(), img4.height());
		auto psrc = img4.getBytes();
		for (size_t i = 0; i < _pixels.size(); ++i) {
			_pixels[i] = (psrc[2] * 299 + psrc[1] * 587 + psrc[0] * 114 + 500) / 1000; // 灰度处理
			psrc += DEEP;
		}
	}
	bool write(LPCTSTR file, bool bin=false) {
		if (empty())
			return false;
		WImage temp;
		temp.create(_width, _height, 3);
		temp.translate(_pixels.data(), 1, _width, bin);
		temp.write(file);
		return true;
	}
	void printData() {
		if (empty()) {
			std::cout << "empty imageBin" << std::endl;
		}
		printf("ImageInfo:%dx%d\n", _width, _height);
		for (size_t i = 0; i < _height; ++i) {
			for (size_t j = 0; j < _width; ++j) {
				printf("%2X ", _pixels[i * _width + j]);
			}
			std::cout << std::endl;
		}
	}
	void fill(const rect_t& rc, BYTE v) {
		int width = rc.width();
		for (int y = rc.y1; y < rc.y2; ++y) {
			memset(ptr(y)+rc.x1, v, sizeof(BYTE) * width);
		}
	}
public:
	iterator begin() {
		return _pixels.data();
	}
	iterator end() {
		return _pixels.data() + _pixels.size();
	}
public:
	int width() const {
		return _width;
	}
	int height() const{
		return _height;
	}
private:
	int _width, _height;
	std::vector<BYTE> _pixels;
};


using InputImg = const WImage&;
using OutputImg = WImage&;

using InputBin = const WImageBin&;
using OutputBin = WImageBin&;
