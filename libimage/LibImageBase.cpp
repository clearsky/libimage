#include "color.h"
#include "LibImageBase.h"
#include "helpfunc.h"
#include <atomic>

LibImageBase::LibImageBase() :m_threadPool(std::thread::hardware_concurrency()) {
	_cur_dic_idx = 0;
}

LibImageBase::~LibImageBase() {
	// 清除加载的图片
	for (auto&& item : _img_cache) {
		if(item.second) delete item.second;
	}
}

long LibImageBase::Capture(const std::wstring& file) {
	return _src.write(file.c_str());
}

long LibImageBase::UseDict(int idx) {
	if (idx < 0 || idx >= _max_dict)  return 0;
	_cur_dic_idx = idx;
	return 1;
}

int LibImageBase::VUINT642Colordfs(const std::vector<UINT64>& dfcolor_pre, std::vector<color_df_t>& colors)
{
	colors.clear();
	color_df_t temp_dfcolor;
	for (auto&& item : dfcolor_pre) {
		temp_dfcolor.color.r = (item & 0xff00000000000000) >> 56;
		temp_dfcolor.color.g = (item & 0x00ff000000000000) >> 48;
		temp_dfcolor.color.b = (item & 0x0000ff0000000000) >> 40;
		temp_dfcolor.df.r = (item & 0x00000000ff000000) >> 24;
		temp_dfcolor.df.g = (item & 0x0000000000ff0000) >> 16;
		temp_dfcolor.df.b = (item & 0x000000000000ff00) >> 8;
		colors.push_back(temp_dfcolor);
	}
	return colors.size();
}

void LibImageBase::UINT322Color(const UINT32 v, color_t& color)
{
	color.r = (v & 0xff000000) >> 24;
	color.g = (v & 0x00ff0000) >> 16;
	color.b = (v & 0x0000ff00) >> 8;
}

long LibImageBase::Ocr(Dict& dict, double sim, std::wstring& ret_str) {
	ret_str.clear();
	std::map<point_t, std::wstring> ps;
	bin_ocr(dict, sim, ps);
	if (!ps.size()) return 0;
	for (auto& it : ps) {
		ret_str += it.second;
	}
	return 1;
}

long LibImageBase::OcrOne(Dict& dict, double sim, std::wstring& ret_str)
{
	ret_str.clear();
	std::map<point_t, std::wstring> ps;
	bin_ocr(dict, sim, ps, true);
	if (!ps.size()) return 0;
	for (auto& it : ps) {
		ret_str += it.second;
	}
	return 1;
}

long LibImageBase::OcrEx(Dict& dict, double sim, std::map<point_t, std::wstring>& ret)
{
	ret.clear();
	bin_ocr(dict, sim, ret);
	if (!ret.size()) return 0;
	return 1;
}

long LibImageBase::FindStrFast(const std::wstring& strs, double sim)
{
	if (strs.empty()) return 0;

	std::map<point_t, std::wstring> ret;
	bin_find_str(strs, sim, ret, true);
	return ret.size() ? 1 : 0;
}

long LibImageBase::FindStrFastEx(const std::wstring& strs, double sim, std::map<point_t, std::wstring>& ret)
{
	if (strs.empty()) return 0;
	bin_find_str(strs, sim, ret);
	return ret.size() ? 1 : 0;
}

long LibImageBase::AddDict(int index, const std::wstring& str)
{
	if (index >= _max_dict || str.empty()) return false;
	bool ret = _dicts[index].add_word_dm(str);
	if (ret) {
		_words_cache[index][(_dicts[index].getWords().end() - 1)->info.name].push_back(_dicts[index].size() - 1);
	}
	return ret;
}

long LibImageBase::LoadPic(const std::wstring& name, WImage* pImg)
{
	if (_img_cache.count(name)) {
		auto it = _img_cache[name];
		if (it) {
			delete it;
			_img_cache[name] = nullptr;
		}
	}
	_img_cache[name] = pImg;
	return 1;
}

long LibImageBase::FindPic(std::vector<WImage*>& pics, color_t dfcolor, double sim, point_desc_t& ret)
{
	long x = 0;
	long y = 0;
	sim = 0.5 + sim / 2;
	int m_simColor = 9;
	if (sim >= 0.8 && sim < 0.9) m_simColor = 18;
	else if (sim >= 0.7 && sim < 0.8) m_simColor = 29;
	else if (sim >= 0.6 && sim < 0.7) m_simColor = 38;
	else if (sim >= 0.5 && sim < 0.6) m_simColor = 49;
	dfcolor.r += m_simColor;
	dfcolor.g += m_simColor;
	dfcolor.b += m_simColor;
	for (int i = 0; i < pics.size(); i++) {
		_find_pic(pics[i], dfcolor, x,y);
		if (x != -1) {
			ret.id = i;
			ret.pos.x = x;
			ret.pos.y = y;
			return 1;
		}
	}
	ret.id = -1;
	ret.pos = { -1,-1 };
	return 0;
}

long LibImageBase::FindPicEx(std::vector<WImage*>& pics, color_t dfcolor, double sim, vpoint_desc_t& vpd)
{
	sim = 0.5 + sim / 2;
	int m_simColor = 9;
	if (sim >= 0.8 && sim < 0.9) m_simColor = 18;
	else if (sim >= 0.7 && sim < 0.8) m_simColor = 29;
	else if (sim >= 0.6 && sim < 0.7) m_simColor = 38;
	else if (sim >= 0.5 && sim < 0.6) m_simColor = 49;
	dfcolor.r += m_simColor;
	dfcolor.g += m_simColor;
	dfcolor.b += m_simColor;
	std::vector<point_t> temp;
	point_desc_t temp_desc;
	for (int i = 0; i < pics.size(); i++) {
		temp.clear();
		_find_pic_ex(pics[i], dfcolor, temp);
		for (auto&& item : temp) {
			temp_desc.id = i;
			temp_desc.pos.x = item.x;
			temp_desc.pos.y =_src.height()- item.y;
			vpd.push_back(temp_desc);
		}
	}
	if (vpd.size()) return 1;
	return 0;
}

void LibImageBase::bin_ocr(const Dict& dict, double sim, std::map<point_t, std::wstring>& ps, bool one) {
	ps.clear();
	if (dict.empty()) return;
	if (_binary.empty()) return;
	_record.create(_binary.width(), _binary.height());
	memset(_record.data(), 0, sizeof(BYTE) * _record.width() * _record.height());
	sim = 0.5 + sim / 2;
	_bin_ocr(dict.getWords(), sim, ps, one);
}

void LibImageBase::bin_find_str(const std::wstring& strs, double sim, std::map<point_t, std::wstring>& ps, bool find_one)
{
	ps.clear();
	if (_binary.empty()) return;
	if (strs.empty()) return;
	std::vector<std::wstring>  vStrs;
	LibImage_help::split(strs, vStrs, L"|");
	if (!vStrs.size()) return;

	_record.create(_binary.width(), _binary.height());
	memset(_record.data(), 0, sizeof(BYTE) * _record.width() * _record.height());
	record_sum(_binary);

	sim = 0.5 + sim / 2;

	std::vector<const Word*> words;
	for (auto&& item : vStrs) {
		if (_words_cache[_cur_dic_idx].count(item)) {
			for (auto&& it : _words_cache[_cur_dic_idx][item]) {
				words.push_back(&(_dicts[_cur_dic_idx].getWords()[it]));
			}
		}
	}
	_bin_ocr(words, sim, ps, find_one);
}

template<typename T>
void LibImageBase::_bin_ocr(const T& words, double sim, std::map<point_t, std::wstring>& ps, bool find_one) {
	int px, py;
	if (_binary.empty()) return;
	record_sum(_binary); // 生成差分矩阵

	int min_bits_count = 255 * 255;
	int max_bits_count = 0;
	int min_width = 255;
	int min_height = 255;
	int max_width = 0;
	int max_height = 0;

	for (auto& it : words) {
		min_bits_count = min(min_bits_count, (*it).info.bit_count);
		max_bits_count = max(max_bits_count, (*it).info.bit_count);
		min_width = min(min_width, (*it).info.w);
		min_height = min(min_height, (*it).info.h);
		max_width = max(max_width, (*it).info.w);
		max_height = max(max_height, (*it).info.h);
	}

	int error = 0;
	point_t pt;
	rect_t crc;
	int real_word_height = 0;
	int real_word_width = 0;
	// 遍历像素点，以每个像素点位原点，通过字库的宽高，生成不同的
	// 区域，用这个区域与字库数据比对
	for (py = 0; py < _binary.height() - min_height; ++py) {
		for (px = 0; px < _binary.width() - min_width; ++px) {
			// 已识别到的区域排除
			if (_record.at(py, px)) continue;
			//像素点过少排除,像素点全匹配也不满足sim
			if (region_sum(px,
				py,
				min(px + max_width, _binary.width()),
				min(py + max_height, _binary.height())) < min_bits_count * sim) {
				continue;
			}
			// 像素点过多排除,像素点全匹配,多余的像素也会撑爆sim,比对对象是像素作对的
			// 后面针对每个具体word的还要排除一次
			if (region_sum(px, py, px + min_width, py + min_height) > max_bits_count * (2 - sim)) {
				continue;
			}
			pt.x = px;
			pt.y = py;
			// 遍历字库
			for (auto& it : words) {
				crc.x1 = px;
				crc.y1 = py;
				crc.x2 = px + (*it).info.w;
				crc.y2 = py + (*it).info.h;
				real_word_height = (*it).info.h;
				real_word_width = (*it).info.w;
				// 边界检查,如果超出边界,就可能匹配不到了
				// 超出的位置可能会被sim和谐
				// 高度越界
				if (crc.y2 > _binary.height()) {
					int d_height = crc.y2 - _binary.height();
					double d_sim = (double)d_height / (*it).info.h;
					// 不可能匹配到了
					if (d_sim > (1. - sim)) {
						continue;
					}
					// 还有机会继续匹配,修正数据
					crc.y2 = _binary.height();
					sim += d_sim;
					// sim超标了,放弃
					if (sim > 1. - 10e-5) {
						continue;
					}
					real_word_height -= d_height;
				}
				// 宽度越界
				if (crc.x2 > _binary.width()) {
					int d_width = crc.x2 - _binary.width();
					double d_sim = (double)d_width / (*it).info.w;
					if (d_sim > (1. - sim)) {
						continue;
					}
					crc.x2 = _binary.width();
					sim += d_sim;
					if (sim > 1. - 10e-5) {
						continue;
					}
					real_word_width -= d_width;
				}

				error = (1. - sim) * real_word_height * real_word_width;
				// 排除像素过多,像素点全匹配,多余的像素也会撑爆sim
				if (abs(region_sum(crc.x1, crc.y1, crc.x2, crc.y2) - (*it).info.bit_count) > error) {
					continue;
				}
				// 匹配
				if (word_part_match(crc, error, (*it).data.data())) {
					pt.y += real_word_height;
					pt.y = _src.height() - pt.y;
					ps[pt] = (*it).info.name;
					_record.fill(crc, 1);
					if (find_one) return;
				}
			}
		}
	}
}
bool LibImageBase::word_part_match(const rect_t& rc, int max_error, const uint8_t* data)
{
	int err_count = 0;
	int index = 0;

	for (int y = rc.y1; y < rc.y2; ++y) {
		for (int x = rc.x1; x < rc.x2; ++x) {
			int val = GET_BIT(data[index >> 3], index & 7);
			if (_binary.at(y, x) ^ val) {
				++err_count;
				if (err_count > max_error) return false;
			}
			++index;
		}
	}
	return true;
}
/*
* _binary是二值化的图,只有黑白色
* _sum纯黑色的24位图
*/
void LibImageBase::record_sum(const WImageBin& _binary)
{
	_sum.create(_binary.width() + 1, _binary.height() + 1, 3);
	_sum.fill(WORD_BKCOLOR);
	int height = _sum.height();
	int width = _sum.height();
	int s = 0;
	for (int i = 1; i < height; ++i) {
		for (int j = 1; j < width; ++j) {
			s = 0;
			s += _sum.at<int>(i - 1, j); //  左边一个像素
			s += _sum.at<int>(i, j - 1); // 下边一个像素
			s -= _sum.at<int>(i - 1, j - 1); // 左下方一个像素
			s += (int)_binary.at(i - 1, j - 1); // 当前位置像素      
			_sum.at<int>(i, j) = s;
		}
	}
}

// 计算像素密度
int LibImageBase::region_sum(int x1, int y1, int x2, int y2)
{
	int ans = _sum.at<INT32>(y2, x2) - _sum.at<INT32>(y2, x1) -
		_sum.at<INT32>(y1, x2) + _sum.at<INT32>(y1, x1);
	return ans;
}
void LibImageBase::_find_pic(WImage* pic, const color_t& dfcolor, long&x, long&y)
{
	// 图片分区
	int thread_count = m_threadPool.getThreadNum();
	std::vector<rect_t> blocks;
	rect_t temp_rect(0, 0, _src.width(), _src.height());
	temp_rect.shrinkRect(pic->width(), pic->height());
	temp_rect.divideBlock(thread_count,
	temp_rect.width() < temp_rect.height(), blocks);
	std::vector<std::future<point_t>> results;
	// 多线程分片识别
	std::atomic_bool stop = false;
	auto func = [this, &pic,&stop, &dfcolor](const rect_t& rc)->point_t {
		bool bad = false;
		int index1 = 0;
		int index2 = 0;
		PBYTE pSrc = _src.getBytes();
		PBYTE pPic = pic->getBytes();
		for (int i = rc.y1; i < rc.y2; ++i) {
			for (int j = rc.x1; j < rc.x2; ++j) {
				if (stop) return{ -1,-1 };
				for (int y1 = 0; y1 < pic->height()&&!bad; ++y1) {
					for (int x1 = 0; x1 < pic->width(); ++x1) {
						index1 = ((i + y1) * _src.width() + j + x1) << 2;
						index2 = (y1 * pic->width() + x1) << 2;
						if (abs(*(pSrc + index1) - *(pPic + index2)) >= (dfcolor.b) ||
							abs(*(pSrc + index1 + 1) - *(pPic + index2 + 1)) >= (dfcolor.g) ||
							abs(*(pSrc + index1 + 2) - *(pPic + index2 + 2)) >= (dfcolor.r)) {
							bad = true;
							break;
						}
					}
				}
				if (bad) {
					bad = false;
					continue;
				}
				stop = true;
				return { j, i+pic->height() };
			}
		}
		return { -1,-1 };
	};
	for (int i = 0; i < thread_count; i++) {
		results.push_back(m_threadPool.enqueue(func, blocks[i]));
	}
	for (auto&& item : results) {
		point_t p = item.get();
		if (p.x != -1) {
			x = p.x;
			y = _src.height() - p.y;
			return;
		}
	}
}
void LibImageBase::_find_pic_ex(WImage* pic, const color_t& dfcolor, std::vector<point_t>& ret)
{
	// 图片分区
	int thread_count = m_threadPool.getThreadNum();
	std::vector<rect_t> blocks;
	rect_t temp_rect(0, 0, _src.width(), _src.height());
	temp_rect.shrinkRect(pic->width(), pic->height());
	temp_rect.divideBlock(thread_count,
		temp_rect.width() < temp_rect.height(), blocks);
	std::vector<std::future<void>> results;
	// 多线程分片识别
	auto func = [this, &pic, &dfcolor,&ret](const rect_t& rc)->void {
		bool bad = false;
		int index1 = 0;
		int index2 = 0;
		PBYTE pSrc = _src.getBytes();
		PBYTE pPic = pic->getBytes();
		for (int i = rc.y1; i < rc.y2; ++i) {
			for (int j = rc.x1; j < rc.x2; ++j) {
				for (int y1 = 0; y1 < pic->height() && !bad; ++y1) {
					for (int x1 = 0; x1 < pic->width(); ++x1) {
						index1 = ((i + y1) * _src.width() + j + x1) << 2;
						index2 = (y1 * pic->width() + x1) << 2;
						if (abs(*(pSrc + index1) - *(pPic + index2)) >= (dfcolor.b) ||
							abs(*(pSrc + index1 + 1) - *(pPic + index2 + 1)) >= (dfcolor.g) ||
							abs(*(pSrc + index1 + 2) - *(pPic + index2 + 2)) >= (dfcolor.r)) {
							bad = true;
							break;
						}
					}
				}
				if (bad) {
					bad = false;
					continue;
				}
				ret.push_back({ j, i + pic->height() });
			}
		}
	};
	for (int i = 0; i < thread_count; i++) {
		results.push_back(m_threadPool.enqueue(func, blocks[i]));
	}
	for (auto&& item : results) {
		item.get();
	}
}
void LibImageBase::bgr2binary(std::vector<color_df_t>& colors) {
	if (_src.empty()) return;
	int ncols = _src.width(), nrows = _src.height();
	_binary.create(_src.width(), _src.height());
	for (int i = 0; i < nrows; ++i) {
		auto psrc = _src.ptr<color_t>(i);
		auto pbin = _binary.ptr(i);
		for (int j = 0; j < ncols; ++j) {
			BYTE g1 = psrc->toGray();
			*pbin = WORD_BKCOLOR;
			for (auto& it : colors) {  //对每个颜色描述
				if (it.compare(*psrc)) {
					*pbin = WORD_COLOR;
					break;
				}
			}
			++pbin;
			++psrc;
		}
	}

}
