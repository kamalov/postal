#include "prelude.h"

struct Data {
    i64 ax;
    i64 ay;
    i64 bx;
    i64 by;
    i64 target_x;
    i64 target_y;
    friend bool operator==(const Data& l, const Data& r) {
        return (l.ax == r.ax) && (l.ay == r.ay) && (l.bx == r.bx) && (l.by == r.by) && (l.target_x == r.target_x) && (l.target_y == r.target_y);
    }
};

namespace std {
    template<>
    struct hash<Data> {
        std::size_t operator()(const Data& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.ax) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ay) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.bx) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.by) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.target_x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.target_y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

struct XY {
    i64 x;
    i64 y;
    friend bool operator==(const XY& l, const XY& r) {
        return (l.x == r.x) && (l.y == r.y);
    }
};

namespace std {
    template<>
    struct hash<XY> {
        std::size_t operator()(const XY& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

XY* get_xy(std::string s) {
    std::vector<std::string>* l;
    std::string x;
    std::string y;
    XY* xy;
    l = str_split(s, " "s);
    x = l->at(2ll);
    x = str_remove(x, "X"s);
    x = str_remove(x, "+"s);
    x = str_remove(x, ","s);
    y = l->at(3ll);
    y = str_remove(y, "Y"s);
    y = str_remove(y, "+"s);
    xy = new XY();
    xy->x = str_to_int(x);
    xy->y = str_to_int(y);
    return xy;
}

Data* create_data(std::vector<std::string>* lines, i64 index) {
    Data* data;
    XY* xy;
    std::vector<std::string>* l;
    std::string x;
    std::string y;
    data = new Data();
    xy = get_xy(lines->at(index));
    data->ax = xy->x;
    data->ay = xy->y;
    xy = get_xy(lines->at(index + 1ll));
    data->bx = xy->x;
    data->by = xy->y;
    l = str_split(lines->at(index + 2ll), " "s);
    x = l->at(1ll);
    x = str_remove(x, "X"s);
    x = str_remove(x, "="s);
    x = str_remove(x, ","s);
    y = l->at(2ll);
    y = str_remove(y, "Y"s);
    y = str_remove(y, "="s);
    data->target_x = str_to_int(x) + 10000000000000ll;
    data->target_y = str_to_int(y) + 10000000000000ll;
    return data;
}

i64 process_data(Data* d) {
    i64 b_up;
    i64 b_down;
    i64 b;
    i64 a_up;
    i64 a_down;
    i64 a;
    i64 price;
    // log(d.ax, d.ay)
    // log(d.bx, d.by)
    // log(d.target_x, d.target_y)
    b_up = d->ax*d->target_y - d->ay*d->target_x;
    b_down = d->ax*d->by - d->ay*d->bx;
    b = b_up/b_down;
    a_up = d->target_x - b*d->bx;
    a_down = d->ax;
    a = a_up/a_down;

    if (a_up%a_down != 0ll || b_up%b_down != 0ll) {
        return 0ll;
    };
    //if a > 100 or b > 100 do ret 0
    price = a*3ll + b;
    printf("%s %lld\n", ("price"s).c_str(), static_cast<i64>(price));
    return price;
}

i64 process(std::vector<std::string>* lines) {
    i64 total;
    i64 index;
    Data* data;
    total = 0ll;

    auto __expr0 = create_range(0ll, len(lines)/4ll);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        index = expr__it0*4ll;
        data = create_data(lines, index);
        total = total + process_data(data);
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    i64 total;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    total = process(lines);
    printf("%s %lld\n", ("total = "s).c_str(), static_cast<i64>(total));
}


