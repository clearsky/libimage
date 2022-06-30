#pragma once
#include <Windows.h>
#include <assert.h>

#include <map>
#include <string>
#include <vector>

template<typename T>
constexpr void SET_BIT(T& x, int idx) {
    x |= 1u << idx;
}
template<typename T>
constexpr int GET_BIT(T x, int idx) {
    return (x >> idx) & 1u;
}

template<typename T>
constexpr int get_bit_count(T x) {
    int s = 0;
    while (x) {
        s += x & 1;
        x >>= 1;
    }
    return s;
}

class NumberGen {
    int _q, _r;

public:
    NumberGen(int n, int cnt) : _q(n / cnt), _r(n% cnt) {}
    int operator[](int idx) const { return idx < _r ? _q + 1 : _q; }
};
struct rect_t {
    rect_t() : x1(0), y1(0), x2(0), y2(0) {}
    rect_t(int x1_, int y1_, int x2_, int y2_)
        : x1(x1_), y1(y1_), x2(x2_), y2(y2_) {}
    int x1, y1;
    int x2, y2;
    int width() const { return x2 - x1; }
    int height() const { return y2 - y1; }
    rect_t& shrinkRect(int w, int h) {
        x2 -= w;
        y2 -= h;
        x2 += 1;
        y2 += 1;
        return *this;
    }
    bool valid() const { return 0 <= x1 && x1 < x2 && 0 <= y1 && y1 < y2; }

    void divideBlock(int count, bool vertical, std::vector<rect_t>& blocks) {
        assert(valid());

        assert(count > 0);
        blocks.resize(count);
        if (vertical) {
            NumberGen gen(height(), count);
            int basey = y1;
            for (int i = 0; i < count; ++i) {
                blocks[i] = rect_t(x1, basey, x2, basey + gen[i]);
                basey += gen[i];
            }

        }
        else {
            NumberGen gen(width(), count);
            int basex = x1;
            for (int i = 0; i < count; ++i) {
                blocks[i] = rect_t(basex, y1, basex + gen[i], y2);
                basex += gen[i];
            }
        }
        assert(blocks.back().x2 == x2);
        assert(blocks.back().y2 == y2);
    }
};

struct point_t {
    int x, y;
    point_t() : x(0), y(0) {}
    point_t(int x_, int y_) : x(x_), y(y_) {}
    bool operator<(const point_t& rhs) const {
        if (std::abs(y - rhs.y) < 9)
            return x < rhs.x;
        else
            return y < rhs.y;
    }
    bool operator==(const point_t& rhs) const { return x == rhs.x && y == rhs.y; }
};
struct point_desc_t {
    int id;
    point_t pos;
};
using vpoint_desc_t = std::vector<point_desc_t>;
#pragma pack(1)
struct FrameInfo {
    unsigned __int64 hwnd;
    unsigned __int32 frameId;
    unsigned __int32 time;
    unsigned __int32 width;
    unsigned __int32 height;
    unsigned __int32 chk;
    void fmtChk() {
        chk = (hwnd >> 32) ^ (hwnd & 0xffffffffull) ^ frameId ^ time ^ width ^ height;
    }

};
#pragma pack()