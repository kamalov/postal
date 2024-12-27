#include "prelude.h"

struct Data {
    i64 x;
    i64 y;
    i64 dx;
    i64 dy;
    friend bool operator==(const Data& l, const Data& r) {
        return (l.x == r.x) && (l.y == r.y) && (l.dx == r.dx) && (l.dy == r.dy);
    }
};

namespace std {
    template<>
    struct hash<Data> {
        std::size_t operator()(const Data& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.dx) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.dy) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

Data* create_data(std::string line) {
    Data* d;
    std::vector<std::string>* ps;
    std::string x;
    std::string y;
    std::vector<std::string>* vs;
    std::string dx;
    std::string dy;
    d = new Data();
    ps = str_split(str_split(line, " "s)->at(0ll), ","s);
    x = str_split(ps->at(0ll), "="s)->at(1ll);
    y = ps->at(1ll);
    d->x = str_to_int(x);
    d->y = str_to_int(y);
    vs = str_split(str_split(line, " "s)->at(1ll), ","s);
    dx = str_split(vs->at(0ll), "="s)->at(1ll);
    dy = vs->at(1ll);
    d->dx = str_to_int(dx);
    d->dy = str_to_int(dy);
    return d;
}

void print(std::vector<Data*>* datas, i64 xlen, i64 ylen) {
    i64 mx;
    i64 my;
    std::string s;
    i64 y;
    i64 x;
    i64 found;
    mx = xlen/2ll;
    my = ylen/2ll;

    auto __expr0 = create_range(0ll, (ylen - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        s = ""s;
        y = expr__it0;

        auto __expr1 = create_range(0ll, (xlen - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            x = expr__it1;
            found = 0ll;

            auto __expr2 = datas;
            for (int datas__it2__idx = 0; datas__it2__idx < __expr2->size(); datas__it2__idx++) {
                Data* datas__it2 = (*__expr2)[datas__it2__idx];

                if (datas__it2->x == x && datas__it2->y == y) {
                    found = 1ll;
                    break;
                };
            }

            if (found) {

                if (x == mx || y == my) {
                    s = s + "#"s;
                }
                else {
                    s = s + "."s;
                };
            }
            else {
                s = s + " "s;
            };
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

i64 go_next(Data* d, i64 xlen, i64 ylen) {
    i64 x;
    i64 y;
    i64 mx;
    i64 my;
    x = (d->x + d->dx) % xlen;

    if (x < 0ll) {
        x = x + xlen;
    };
    y = (d->y + d->dy) % ylen;

    if (y < 0ll) {
        y = y + ylen;
    };
    d->x = x;
    d->y = y;
    mx = xlen/2ll;
    my = ylen/2ll;
    //if x = mx or y = my do ret 0

    if (x == mx) {
        return 0ll;
    };

    if (x > mx && y < my) {
        return 1ll;
    };

    if (x < mx && y < my) {
        return 2ll;
    };

    if (x < mx && y > my) {
        return 3ll;
    };

    if (x > mx && y > my) {
        return 4ll;
    };
    return 6ll;
}

i64 abs1(i64 a) {

    if (a < 0ll) {
        return 0ll - a;
    };
    return a;
}

i64 process(std::vector<std::string>* lines) {
    i64 xlen;
    i64 ylen;
    std::vector<Data*>* datas;
    Data* data;
    std::vector<i64>* quads;
    i64 q1;
    i64 q2;
    i64 q3;
    i64 q4;
    i64 q0;
    i64 q;
    i64 total;
    xlen = 101ll;
    ylen = 103ll;
    datas = new std::vector<Data*>();

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        data = create_data(lines__it0);
        push(datas, data);
    }
    quads = new std::vector<i64>();
    arr_set_len(quads, 5ll);

    auto __expr1 = create_range(1ll, 100000000ll);
    for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
        i64 expr__it1 = (*__expr1)[expr__it1__idx];
        q1 = 0ll;
        q2 = 0ll;
        q3 = 0ll;
        q4 = 0ll;
        q0 = 0ll;

        auto __expr2 = datas;
        for (int datas__it2__idx = 0; datas__it2__idx < __expr2->size(); datas__it2__idx++) {
            Data* datas__it2 = (*__expr2)[datas__it2__idx];
            q = go_next(datas__it2, xlen, ylen);

            if (q == 0ll) {
                q0 = q0 + 1ll;
            };

            if (q == 1ll) {
                q1 = q1 + 1ll;
            };

            if (q == 2ll) {
                q2 = q2 + 1ll;
            };

            if (q == 3ll) {
                q3 = q3 + 1ll;
            };

            if (q == 4ll) {
                q4 = q4 + 1ll;
            };
        }

        if (expr__it1 % 100000ll == 0ll) {
            printf("%lld\n", static_cast<i64>(expr__it1));
        };
        //if abs1(q1 - q2) = 0 and abs1(q3 - q4) = 0 {

        if (q0 > 24ll) {
            print(datas, xlen, ylen);
            //log('step', it, abs1(q1 - q2), abs1(q3 - q4))
            printf("%s %lld %s %lld\n", ("step"s).c_str(), static_cast<i64>(expr__it1), ("q0"s).c_str(), static_cast<i64>(q0));
            readln();
        };
    }
    total = q1*q2*q3*q4;
    return total;
}

void run() {
    std::vector<std::string>* lines;
    i64 total;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    total = process(lines);
    printf("%s %lld\n", ("total = "s).c_str(), static_cast<i64>(total));
}


