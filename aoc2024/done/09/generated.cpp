#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

/// lib array utils
template <typename T>long long len(std::vector<T>* a);
template <typename T>void set_array_size(std::vector<T>* a, long long new_size);
template <typename T>void push(std::vector<T>* a, T elem);
template <typename T>void set_array_value(std::vector<T>* a, long long index, T value);
template <typename T>void sort_array(std::vector<T>* a);
template <typename T>long long array_contains(std::vector<T>* a, T value);
template <typename T>void remove_array_element_at(std::vector<T>* a, long long index);
/// lib string utils
std::vector<std::string>* split_str(std::string s, std::string by);
long long str_to_int(std::string s);
std::string int_to_str(long long i);
std::vector<std::string>* str_to_chars(std::string s);
long long str_contains(std::string s, std::string subs);
/// lib other utils
void err(std::string s);
std::string read_line_from_console();
std::vector<std::string>* read_string_lines_from_file(std::string filename);
struct FileOrGap {
    long long id;
    long long size;
    long long pos;
};

void print_file_or_gap_list(std::vector<FileOrGap*>* vals) {
    std::string s;
    std::string v;
    s = ""s;

    auto __expr0 = vals;
    for (int vals__it0__idx = 0; vals__it0__idx < __expr0->size(); vals__it0__idx++) {
        FileOrGap* vals__it0 = (*__expr0)[vals__it0__idx];

        if (vals__it0->id == 0ll) {
            v = "."s;
        }
        else {
            v = int_to_str(vals__it0->id - 1ll);
        };

        auto __expr1 = create_range(1ll, vals__it0->size);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            s = s + v;
        }
    }
    printf("%s\n", (s).c_str());
}

void print_plains(std::vector<long long>* vals) {
    std::string s;
    std::string v;
    s = ""s;

    auto __expr0 = vals;
    for (int vals__it0__idx = 0; vals__it0__idx < __expr0->size(); vals__it0__idx++) {
        long long vals__it0 = (*__expr0)[vals__it0__idx];

        if (vals__it0 == 0ll) {
            v = "."s;
        }
        else {
            v = int_to_str(vals__it0 - 1ll);
        };
        s = s + v;
    }
    printf("%s\n", (s).c_str());
}

struct GapsPosition {
    std::vector<long long>* gaps;
};

void print_gaps(std::vector<GapsPosition*>* gaps_by_len) {
    std::string s;

    auto __expr0 = gaps_by_len;
    for (int gaps_by_len__it0__idx = 0; gaps_by_len__it0__idx < __expr0->size(); gaps_by_len__it0__idx++) {
        GapsPosition* gaps_by_len__it0 = (*__expr0)[gaps_by_len__it0__idx];
        s = int_to_str(gaps_by_len__it0__idx) + " -> "s;

        auto __expr1 = gaps_by_len__it0->gaps;
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            s = s + " "s + int_to_str(expr__it1);
        }
        printf("%s\n", (s).c_str());
    }
}

std::vector<GapsPosition*>* get_gaps(std::vector<FileOrGap*>* items) {
    std::vector<GapsPosition*>* gaps_by_len = new std::vector<GapsPosition*>();
    GapsPosition* g = new GapsPosition();
    gaps_by_len = new std::vector<GapsPosition*>();

    auto __expr0 = create_range(0ll, 9ll);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        long long expr__it0 = (*__expr0)[expr__it0__idx];
        g = new GapsPosition();
        g->gaps = new std::vector<long long>();
        push(gaps_by_len, g);
    }

    auto __expr1 = items;
    for (int items__it1__idx = 0; items__it1__idx < __expr1->size(); items__it1__idx++) {
        FileOrGap* items__it1 = (*__expr1)[items__it1__idx];

        if (items__it1->id == 0ll) {

            if (items__it1->size > 0ll) {
                push(gaps_by_len->at(items__it1->size)->gaps, items__it1->pos);
                //break
            };
        };
    }
    return gaps_by_len;
}

std::vector<long long>* defrag(std::vector<FileOrGap*>* items, std::vector<long long>* old_plains) {
    std::vector<long long>* plains;
    std::vector<GapsPosition*>* gaps_by_len = new std::vector<GapsPosition*>();
    long long fi;
    long long count;
    FileOrGap* f = new FileOrGap();
    long long min_pos;
    long long gap_size;
    std::vector<long long>* gaps;
    long long pos;
    long long diff;
    std::vector<long long>* new_gaps;
    plains = new std::vector<long long>();
    set_array_size(plains, len(old_plains));
    gaps_by_len = get_gaps(items);
    //print_gaps(gaps_by_len)
    fi = len(items) - 1ll;
    count = 0ll;

    for (;;) {
        count = count + 1ll;

        if (count%1000ll == 0ll) {
            printf("%lld\n", static_cast<long long>(count));
        };

        if (fi < 0ll) {
            break;
        };
        f = items->at(fi);

        if (f->id == 0ll) {
            fi = fi - 1ll;
            continue;
        };
        min_pos = 1000000ll;
        gap_size = 0ll;

        auto __expr0 = create_range((f->size), 9ll);
        for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
            long long expr__it0 = (*__expr0)[expr__it0__idx];
            gaps = gaps_by_len->at(expr__it0)->gaps;

            if (len(gaps) > 0ll) {
                pos = gaps->at(0ll);

                if (pos < f->pos && pos < min_pos) {
                    min_pos = pos;
                    gap_size = expr__it0;
                };
            };
        }

        if (gap_size > 0ll) {
            gaps = gaps_by_len->at(gap_size)->gaps;
            pos = gaps->at(0ll);
            remove_array_element_at(gaps, 0ll);
            diff = gap_size - f->size;

            auto __expr1 = create_range(0ll, (f->size - 1ll));
            for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
                long long expr__it1 = (*__expr1)[expr__it1__idx];
                set_array_value(plains, pos + expr__it1, f->id);
            }

            if (diff > 0ll) {
                new_gaps = gaps_by_len->at(diff)->gaps;
                push(new_gaps, pos + f->size);
                sort_array(new_gaps);
            };
        };

        if (gap_size == 0ll) {

            auto __expr2 = create_range(0ll, (f->size - 1ll));
            for (int expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
                long long expr__it2 = (*__expr2)[expr__it2__idx];
                set_array_value(plains, f->pos + expr__it2, f->id);
            }
        };
        //print_plains(plains)
        // // print_gaps(gaps_by_len)
        //read_line_from_console()
        fi = fi - 1ll;
    }
    return plains;
}

long long process(std::string line) {
    std::vector<FileOrGap*>* vals = new std::vector<FileOrGap*>();
    long long is_file;
    long long id;
    std::vector<std::string>* chars;
    std::vector<long long>* plains;
    long long pos;
    long long size;
    FileOrGap* f = new FileOrGap();
    std::vector<long long>* new_plains;
    long long total;
    vals = new std::vector<FileOrGap*>();
    is_file = 1ll;
    id = 1ll;
    chars = str_to_chars(line);
    plains = new std::vector<long long>();
    pos = 0ll;

    auto __expr0 = chars;
    for (int chars__it0__idx = 0; chars__it0__idx < __expr0->size(); chars__it0__idx++) {
        std::string chars__it0 = (*__expr0)[chars__it0__idx];
        size = str_to_int(chars__it0);
        f = new FileOrGap();
        f->pos = pos;
        f->size = size;

        if (is_file) {
            f->id = id;
            id = id + 1ll;
        }
        else {
            f->id = 0ll;
        };

        auto __expr1 = create_range(1ll, size);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            push(plains, f->id);
        }
        push(vals, f);
        pos = pos + size;
        is_file = is_file == 0ll;
    }
    print_file_or_gap_list(vals);
    print_plains(plains);
    new_plains = defrag(vals, plains);
    print_plains(new_plains);
    total = 0ll;

    auto __expr2 = new_plains;
    for (int new_plains__it2__idx = 0; new_plains__it2__idx < __expr2->size(); new_plains__it2__idx++) {
        long long new_plains__it2 = (*__expr2)[new_plains__it2__idx];

        if (new_plains__it2 == 0ll) {
            continue;
        };
        total = total + (new_plains__it2 - 1ll)*new_plains__it2__idx;
    }
    return total;
}

/// main
void run() {
    std::vector<std::string>* lines;
    long long count;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    count = process(lines->at(0ll));
    printf("%lld %s\n", static_cast<long long>(count), ("done"s).c_str());
}



int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 10);

    try {
        run();
    } catch (const std::string& ex) {
        SetConsoleTextAttribute(hConsole, 12);
        printf("error: %s\n", ex.c_str());
        return -1;
    } catch (const std::exception& e) {
        SetConsoleTextAttribute(hConsole, 12);
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
    
    SetConsoleTextAttribute(hConsole, 7);
    return 0;
}
