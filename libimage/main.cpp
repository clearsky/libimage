#include <iostream>
#include "WImage.h"
#include <time.h>
#include "LibImageBase.h"
#include "color.h"
#include "WGdi.h"
#include <Gdiplus.h>
#include <math.h>
#include "LibImage.h"

int main() {
	long total_start = GetTickCount64();
	LibImage lib(true);
	//lib.Bind((HWND)29302006);
	long init_end = GetTickCount64();
	lib.LoadPic(L"test1", L"C:\\Users\\Administrator\\Desktop\\testn.bmp");
	lib.LoadPic(L"test2", L"C:\\Users\\Administrator\\Desktop\\testn1.bmp");
	long load_pic_end = GetTickCount64();
	point_desc_t find_pic_ret;
	lib.FindPic(0, 0, 1920, 1080, { L"test1", L"test2" }, 0x00000000, 0.9, find_pic_ret);
	long find_pic_end = GetTickCount64();
	vpoint_desc_t find_picex_ret;
	lib.FindPicEx(0, 0, 1920, 1080, { L"test1", L"test2" }, 0x00000000, 0.9, find_picex_ret);
	long find_picex_end = GetTickCount64();

	lib.AddDict(7, L"88C00082D55AAB556AAD5520A400801012004088010C6184408110020200000060000000040$高级选项$0.0.65$11");
	lib.AddDict(7, L"FFFFFFFFFFFFFFFFFFFFE1F83E03C0780F07E0FC0780F01E03F07F0FE3FFFFFFFFFFE$回收站$0.0.355$25");
	lib.AddDict(7, L"440200011502A0550801004509000000010000200400AA554AA9552AA557C$微信$0.0.57$11");

	lib.UseDict(7);
	long add_dict_end = GetTickCount64();
	std::wstring ocr_one_ret;
	lib.OcrOne(0, 0, 1920, 1080, { 0xffffff0030303000 }, 0.8, ocr_one_ret);
	long ocr_one_end = GetTickCount64();
	std::wstring ocr_ret;
	lib.Ocr(0, 0, 1920, 1080, { 0xffffff0030303000 }, 0.8, ocr_ret);
	long ocr_end = GetTickCount64();
	std::map<point_t, std::wstring> ocrex_ret;
	lib.OcrEx(0, 0, 1920, 1080, { 0xffffff0030303000 }, 0.8, ocrex_ret);
	long ocrex_end = GetTickCount64();
	bool find_str_fast_ret = lib.FindStrFast(0, 0, 1920, 1080, L"微信|回收站|高级选项", { 0xffffff0030303000 }, 0.8);
	long find_str_fast_end = GetTickCount64();
	std::map<point_t, std::wstring> find_str_fastex_ret;
	lib.FindStrFastEx(0, 0, 1920, 1080, L"微信|回收站|高级选项",{ 0xffffff0030303000 }, 0.8, find_str_fastex_ret);
	long find_str_fastex_end = GetTickCount64();
	lib.Capture(0, 0, 500, 500, L"./test_cache/capture.bmp");
	long capture_end = GetTickCount64();
	lib.CapturePre(L"./test_cache/capturePre.bmp");
	long capturePre_end = GetTickCount64();

	printf("int use:%dms\n", init_end - total_start);
	printf("load img use:%dms\n", load_pic_end - init_end);
	printf("FindPicRet: (%d,%d,%d)\n", find_pic_ret.id, find_pic_ret.pos.x, find_pic_ret.pos.y);
	printf("FindPic use:%dms\n", find_pic_end - load_pic_end);
	printf("FindPicExRet:\n");
	for (auto&& item : find_picex_ret) {
		printf("----(%d,%d,%d)\n", item.id, item.pos.x, item.pos.y);
	}
	printf("FindPicEx use:%dms\n", find_picex_end - find_pic_end);
	printf("AddDitc use:%dms\n", add_dict_end - find_picex_end);
	wprintf(L"OcrOneRet:%s", ocr_one_ret.c_str());
	printf("OcrOne use:%dms\n", ocr_one_end - add_dict_end);
	wprintf(L"OcrRet:%s", ocr_ret.c_str());
	printf("Ocr use:%dms\n", ocr_end - ocr_one_end);
	printf("OcrExRet:\n");
	for (auto&& item : ocrex_ret) {
		wprintf(L"----(%d, %d), %s\n", item.first.x, item.first.y, item.second.c_str());
	}
	printf("OcrEx use:%dms\n", ocrex_end - ocr_end);
	printf("FindStrFastRet:%d\n", find_str_fast_ret);
	printf("FindStrFast use:%dms\n", find_str_fast_end - ocrex_end);
	printf("FindStrFastExRet:\n");
	for (auto&& item : find_str_fastex_ret) {
		wprintf(L"----(%d, %d), %s\n", item.first.x, item.first.y, item.second.c_str());
	}
	printf("FindStrFastEx use:%dms\n", find_str_fastex_end - find_str_fast_end);
	printf("Capture use:%dms\n", capture_end - find_str_fastex_end);
	printf("CapturePre use:%dms\n", capturePre_end - capture_end);
	printf("Total use:%dms\n", capturePre_end - total_start);

	system("pause");
}