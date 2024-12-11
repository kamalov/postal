#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

/// lib array utils
template <typename T>long long len(std::vector<T>* a);
template <typename T>void set_size(std::vector<T>* a, long long new_size);
template <typename T>void push(std::vector<T>* a, T elem);
template <typename T>void set_value(std::vector<T>* a, long long index, T value);
template <typename T>void sort(std::vector<T>* a);
/// lib file and string utils
std::vector<std::string>* read_lines(std::string filename);
std::vector<std::string>* split_str(std::string s, std::string by);
long long str_to_int(std::string s);
std::string int_to_str(long long i);
std::vector<std::string>* str_to_chars(std::string s);
long long str_contains_str(std::string s, std::string subs);
/// main
void log_int_array(std::vector<long long>* a) {
    std::string s;
    s = ""s;

    auto _expr0 = a;
    for (int a__it0__idx = 0; a__it0__idx < _expr0->size(); a__it0__idx++) {
        long long a__it0 = (*_expr0)[a__it0__idx];
        s = s + " "s + int_to_str(a__it0);
    }
    printf("%s\n", (s).c_str());
}

long long get_pages_rank(std::vector<long long>* pages, std::vector<long long>* rules) {
    long long incorrect;
    long long swapped;
    long long i;
    long long a;
    long long b;
    long long rule;
    log_int_array(pages);
    incorrect = 0ll;

    for (;;) {
        swapped = 0ll;

        auto _expr0 = create_range(0ll, (len(pages) - 2ll));
        for (int exp__it0__idx = 0; exp__it0__idx < _expr0->size(); exp__it0__idx++) {
            long long exp__it0 = (*_expr0)[exp__it0__idx];
            i = exp__it0;
            a = pages->at(i);
            b = pages->at(i + 1ll);
            rule = b*100ll + a;

            if (rules->at(rule)) {
                set_value(pages, i, b);
                set_value(pages, i + 1ll, a);
                swapped = 1ll;
            };
        }

        if (swapped == 0ll) {
            break;
        };
        incorrect = 1ll;
    }
    log_int_array(pages);
    printf("\n");

    if (incorrect) {
        return pages->at(len(pages)/2ll);
    };
    return 0ll;
}

void run() {
    std::vector<std::string>* lines;
    std::vector<long long>* rules;
    long long total;
    std::vector<std::string>* parts;
    long long rule;
    std::vector<long long>* pages;
    std::vector<std::string>* pages_str;
    lines = read_lines("D:/src/postal/aoc2024/input.txt"s);
    rules = new std::vector<long long>();
    set_size(rules, 10000ll);
    total = 0ll;

    auto _expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < _expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*_expr0)[lines__it0__idx];

        if (lines__it0 == ""s) {
            continue;
        };

        if (str_contains_str(lines__it0, "|"s)) {
            parts = split_str(lines__it0, "|"s);
            rule = str_to_int(parts->at(0ll))*100ll + str_to_int(parts->at(1ll));
            set_value(rules, rule, 1ll);
        };

        if (str_contains_str(lines__it0, ","s)) {
            pages = new std::vector<long long>();
            pages_str = split_str(lines__it0, ","s);

            auto _expr1 = pages_str;
            for (int pages_str__it1__idx = 0; pages_str__it1__idx < _expr1->size(); pages_str__it1__idx++) {
                std::string pages_str__it1 = (*_expr1)[pages_str__it1__idx];
                push(pages, str_to_int(pages_str__it1));
            }
            total = total + get_pages_rank(pages, rules);
        };
    }
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
        printf("%s\n", ex.c_str());
        return -1;
    } catch (...) {
        SetConsoleTextAttribute(hConsole, 12);
        printf("exception\n");
        return -1;
    }
    
    SetConsoleTextAttribute(hConsole, 7);
    return 0;
}
