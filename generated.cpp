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
struct Val {
    long long value;
    long long count;
};

Val* create_new_val(long long value, long long count) {
    Val* v = new Val();
    v = new Val();
    v->value = value;
    v->count = count;
    return v;
}

void print(std::vector<Val*>* vals) {
    std::string s;
    s = ""s;

    auto __expr0 = vals;
    for (int vals__it0__idx = 0; vals__it0__idx < __expr0->size(); vals__it0__idx++) {
        Val* vals__it0 = (*__expr0)[vals__it0__idx];

        if (vals__it0->count == 0ll) {
            continue;
        };
        s = s + " "s + int_to_str(vals__it0->value) + " ("s + int_to_str(vals__it0->count) + ")"s;
    }
    printf("%s\n", (s).c_str());
}

std::vector<Val*>* get_new_vals(std::vector<Val*>* old_vals) {
    std::vector<Val*>* new_vals = new std::vector<Val*>();
    Val* old_val = new Val();
    long long value;
    long long count;
    long long size;
    long long exponent;
    long long divider;
    long long lv;
    long long rv;
    long long new_value;
    std::string s;
    Val* new_val = new Val();
    long long found;
    new_vals = new std::vector<Val*>();

    auto __expr0 = old_vals;
    for (int old_vals__it0__idx = 0; old_vals__it0__idx < __expr0->size(); old_vals__it0__idx++) {
        Val* old_vals__it0 = (*__expr0)[old_vals__it0__idx];
        old_val = old_vals__it0;
        value = old_vals__it0->value;
        count = old_vals__it0->count;

        if (count == 0ll) {
            continue;
        };
        old_val->count = 0ll;

        if (value == 0ll) {
            push(new_vals, create_new_val(1ll, count));
            continue;
        };

        if (value == 1ll) {
            push(new_vals, create_new_val(2024ll, count));
            continue;
        };
        size = len(str_to_chars(int_to_str(value)));

        if (size%2ll == 0ll) {
            exponent = size/2ll;
            divider = 1ll;

            auto __expr1 = create_range(1ll, exponent);
            for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
                long long expr__it1 = (*__expr1)[expr__it1__idx];
                divider = divider*10ll;
            }
            lv = value/divider;
            rv = value%divider;
            push(new_vals, create_new_val(lv, count));
            push(new_vals, create_new_val(rv, count));
            continue;
        };
        new_value = value*2024ll;

        if (new_value < 0ll) {
            s = "overflow at "s + int_to_str(value) + " "s + int_to_str(new_value);
            err(s);
        };
        push(new_vals, create_new_val(new_value, count));
    }

    auto __expr2 = new_vals;
    for (int new_vals__it2__idx = 0; new_vals__it2__idx < __expr2->size(); new_vals__it2__idx++) {
        Val* new_vals__it2 = (*__expr2)[new_vals__it2__idx];
        new_val = new_vals__it2;
        found = 0ll;

        auto __expr3 = old_vals;
        for (int old_vals__it3__idx = 0; old_vals__it3__idx < __expr3->size(); old_vals__it3__idx++) {
            Val* old_vals__it3 = (*__expr3)[old_vals__it3__idx];

            if (old_vals__it3->value == new_val->value) {
                old_vals__it3->count = old_vals__it3->count + new_val->count;
                found = 1ll;
                break;
            };
        }

        if (found == 0ll) {
            push(old_vals, create_new_val(new_val->value, new_val->count));
        };
    }
    return old_vals;
    // new_vals = [Val]
    // for old_vals {
    //    if it.count > 0 do push(new_vals, it)
    // }
    // for new_values {
    //    push(new_vals, create_new_val(it, 1))
    // }
    // ret new_vals
}

long long get_len(std::vector<Val*>* vals) {
    long long total;
    total = 0ll;

    auto __expr0 = vals;
    for (int vals__it0__idx = 0; vals__it0__idx < __expr0->size(); vals__it0__idx++) {
        Val* vals__it0 = (*__expr0)[vals__it0__idx];

        if (vals__it0->count > 0ll) {
            total = total + vals__it0->count;
        };
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    std::vector<Val*>* vals = new std::vector<Val*>();
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    vals = new std::vector<Val*>();

    auto __expr0 = split_str(lines->at(0ll), " "s);
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        std::string expr__it0 = (*__expr0)[expr__it0__idx];
        push(vals, create_new_val(str_to_int(expr__it0), 1ll));
    }
    print(vals);

    auto __expr1 = create_range(1ll, 75ll);
    for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
        long long expr__it1 = (*__expr1)[expr__it1__idx];
        vals = get_new_vals(vals);
        printf("%lld %lld %lld\n", static_cast<long long>(expr__it1), static_cast<long long>(len(vals)), static_cast<long long>(get_len(vals)));
        // print(vals)
        // read_line_from_console()
    }
    printf("%s %lld %lld\n", ("done"s).c_str(), static_cast<long long>(len(vals)), static_cast<long long>(get_len(vals)));
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
