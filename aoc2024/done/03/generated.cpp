#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

std::vector<std::string>* read_lines(std::string filename);
std::vector<std::string>* split_str(std::string s, std::string by);
long long str_to_int(std::string s);
std::string int_to_str(long long i);
std::vector<std::string>* str_to_chars(std::string s);
// fn sort_int_array(a: [int]) [int] external
template <typename T>long long len(std::vector<T*>* a);
struct Parser {
    long long current_index;
    std::vector<std::string>* chars;
};

std::string get_char_at_index(Parser* parser, long long index) {

    if (index >= parser->chars->size()) {
        return ""s;
    };
    return parser->chars->at(index);
}

long long try_skip(Parser* p, std::string s) {
    std::vector<std::string>* chars;
    std::string c;
    chars = str_to_chars(s);

    for (int chars__it0__idx = 0; chars__it0__idx < chars->size(); chars__it0__idx++) {
        std::string chars__it0 = chars->at(chars__it0__idx);
        c = get_char_at_index(p, p->current_index + chars__it0__idx);

        if (c != chars__it0) {
            return 0;
        };
    }
    p->current_index = p->current_index + chars->size();
    return 1;
}

std::string peek_next(Parser* p) {
    return get_char_at_index(p, p->current_index);
}

std::string get_next(Parser* p) {
    std::string s;
    s = get_char_at_index(p, p->current_index);
    p->current_index = p->current_index + 1;
    return s;
}

long long is_numeric(std::string s) {

    if (s == "0"s || s == "1"s || s == "2"s || s == "3"s || s == "4"s || s == "5"s || s == "6"s || s == "7"s || s == "8"s || s == "9"s) {
        return 1;
    };
    return 0;
}

std::string get_next_numeric_str(Parser* p) {
    std::string result;
    std::string s;
    result = ""s;

    for (;;) {
        s = peek_next(p);

        if (is_numeric(s)) {
            result = result + s;
            get_next(p);
        }
        else {
            break;
        };
    }
    return result;
}

long long get_total(std::string line) {
    Parser* p = new Parser();
    long long is_on;
    long long total;
    std::string s1;
    std::string s2;
    long long v1;
    long long v2;
    long long v;
    std::string s;
    p = new Parser();
    p->chars = str_to_chars(line);
    is_on = 1;
    total = 0;

    for (;;) {

        if (peek_next(p) == ""s) {
            break;
        };

        if (try_skip(p, "do()"s)) {
            is_on = 1;
            continue;
        };

        if (try_skip(p, "don't()"s)) {
            is_on = 0;
            continue;
        };

        if (try_skip(p, "mul("s)) {
            s1 = get_next_numeric_str(p);

            if (s1 == ""s) {
                continue;
            };

            if (try_skip(p, ","s) == 0) {
                continue;
            };
            s2 = get_next_numeric_str(p);

            if (s2 == ""s) {
                continue;
            };

            if (try_skip(p, ")"s) == 0) {
                continue;
            };
            v1 = str_to_int(s1);
            v2 = str_to_int(s2);
            v = v1*v2;
            printf("%s %lld\n", ("mul("s + s1 + ","s + s2 + ") = "s).c_str(), static_cast<long long>(v));

            if (is_on) {
                total = total + v;
            };
        }
        else {
            s = get_next(p);
        };
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    std::string line;
    long long total;
    lines = read_lines("D:/src/postal/aoc2024/input.txt"s);
    line = ""s;

    for (int lines__it0__idx = 0; lines__it0__idx < lines->size(); lines__it0__idx++) {
        std::string lines__it0 = lines->at(lines__it0__idx);
        line = line + lines__it0;
    }
    total = get_total(line);
    printf("%s %lld\n", ("total = "s).c_str(), static_cast<long long>(total));
    printf("%s\n", ("done"s).c_str());
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
