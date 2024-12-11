#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

/// array utils
template <typename T>long long len(std::vector<T>* a);
template <typename T>void set_size(std::vector<T>* a, long long new_size);
template <typename T>void push(std::vector<T>* a, T elem);
template <typename T>void set_value(std::vector<T>* a, long long index, T value);
template <typename T>void sort(std::vector<T>* a);
/// file and string utils
std::vector<std::string>* read_lines(std::string filename);
std::vector<std::string>* split_str(std::string s, std::string by);
long long str_to_int(std::string s);
std::string int_to_str(long long i);
std::vector<std::string>* str_to_chars(std::string s);
std::vector<std::string>* str_contains_str(std::string s, std::string subs);
long long get_pages_rank(std::vector<long long>* pages, std::vector<long long>* rules) {
    long long page_index;
    long long page;
    long long rule;
    long long i;

    for (int pages__it0__idx = 0; pages__it0__idx < pages->size(); pages__it0__idx++) {
        long long pages__it0 = pages->at(pages__it0__idx);

        if (pages__it0__idx == 0ll) {
            continue;
        };
        page_index = pages__it0__idx;
        page = pages__it0;

        for (int pages__it1__idx = 0; pages__it1__idx < pages->size(); pages__it1__idx++) {
            long long pages__it1 = pages->at(pages__it1__idx);

            if (pages__it1__idx >= page_index) {
                break;
            };
            rule = page*100ll + pages__it1;

            for (int rules__it2__idx = 0; rules__it2__idx < rules->size(); rules__it2__idx++) {
                long long rules__it2 = rules->at(rules__it2__idx);

                if (rule == rules__it2) {
                    return 0ll;
                };
            }
        }
    }
    i = pages->at(len(pages)/2ll);
    return i;
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

    for (int lines__it0__idx = 0; lines__it0__idx < lines->size(); lines__it0__idx++) {
        std::string lines__it0 = lines->at(lines__it0__idx);

        if (lines__it0 == ""s) {
            continue;
        };

        if (str_contains_str(lines__it0, "|"s)) {
            parts = split_str(lines__it0, "|"s);
            rule = str_to_int(parts->at(0ll))*100ll + str_to_int(parts->at(1ll));
            //push(rules, rule)
        };

        if (str_contains_str(lines__it0, ","s)) {
            pages = new std::vector<long long>();
            pages_str = split_str(lines__it0, ","s);

            for (int pages_str__it1__idx = 0; pages_str__it1__idx < pages_str->size(); pages_str__it1__idx++) {
                std::string pages_str__it1 = pages_str->at(pages_str__it1__idx);
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
