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
/// main
struct IntBox {
    long long value;
};

struct IntField {
    long long row_count;
    long long col_count;
    std::vector<long long>* values;
};

IntField* create_int_field(long long row_count, long long col_count) {
    IntField* f = new IntField();
    f = new IntField();
    f->row_count = row_count;
    f->col_count = col_count;
    f->values = new std::vector<long long>();
    set_array_size(f->values, row_count*col_count);
    return f;
}

long long get_int_field_value(IntField* f, long long row_index, long long col_index) {
    return f->values->at(row_index*f->col_count + col_index);
}

void set_int_field_value(IntField* f, long long row_index, long long col_index, long long value) {
    f->values->at(row_index*f->col_count + col_index) = value;
}

long long is_valid_field_index(IntField* f, long long row_index, long long col_index) {
    return row_index >= 0ll && row_index < f->row_count && col_index >= 0ll && col_index < f->col_count;
}

void print_int_field(IntField* f) {
    long long ri;
    std::string s;
    long long ci;
    long long v;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        long long expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;
        s = ""s;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);
            s = s + " "s + int_to_str(v);
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

IntField* lines_to_int_field(std::vector<std::string>* lines) {
    IntField* f = new IntField();
    long long ri;
    long long ci;
    long long v;
    f = create_int_field(len(lines), len(str_to_chars(lines->at(0ll))));

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        ri = lines__it0__idx;

        auto __expr1 = str_to_chars(lines__it0);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            std::string expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1__idx;
            v = str_to_int(expr__it1);
            set_int_field_value(f, ri, ci, v);
        }
    }
    return f;
}

struct Visited {
    std::vector<long long>* list;
    long long count;
};

void try_visit(IntField* f, long long ri, long long ci, long long target_value, Visited* visited) {
    long long v;
    long long h;

    if (is_valid_field_index(f, ri, ci) == 0ll) {
        return;
    };
    v = get_int_field_value(f, ri, ci);

    if (v != target_value) {
        return;
    };

    if (v == 9ll) {
        visited->count = visited->count + 1ll;
        h = ri*1000ll + ci;

        if (array_contains(visited->list, h) == 0ll) {
            push(visited->list, h);
        };
        return;
    };
    try_visit(f, ri - 1ll, ci, target_value + 1ll, visited);
    try_visit(f, ri + 1ll, ci, target_value + 1ll, visited);
    try_visit(f, ri, ci - 1ll, target_value + 1ll, visited);
    try_visit(f, ri, ci + 1ll, target_value + 1ll, visited);
}

long long get_trails_count(IntField* f, long long ri, long long ci) {
    Visited* visited = new Visited();
    visited = new Visited();
    visited->list = new std::vector<long long>();
    try_visit(f, ri, ci, 0ll, visited);
    return visited->count;
}

long long process_field(IntField* f) {
    long long total;
    long long ri;
    long long ci;
    long long v;
    total = 0ll;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        long long expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_int_field_value(f, ri, ci);

            if (v == 0ll) {
                total = total + get_trails_count(f, ri, ci);
            };
        }
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    IntField* f = new IntField();
    long long total;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    f = lines_to_int_field(lines);
    print_int_field(f);
    total = process_field(f);
    printf("%lld %s\n", static_cast<long long>(total), ("done"s).c_str());
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
