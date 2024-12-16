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
template <typename T>void sort(std::vector<T>* a);
template <typename T>long long array_contains(std::vector<T>* a, T value);
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
struct Data {
    long long target;
    std::vector<long long>* values;
};

std::vector<Data*>* lines_to_datas(std::vector<std::string>* lines) {
    std::vector<Data*>* datas = new std::vector<Data*>();
    Data* data = new Data();
    std::vector<std::string>* parts;
    std::vector<std::string>* target_parts;
    datas = new std::vector<Data*>();

    auto _expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < _expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*_expr0)[lines__it0__idx];
        data = new Data();
        data->values = new std::vector<long long>();
        parts = split_str(lines__it0, " "s);
        target_parts = split_str(parts->at(0ll), ":"s);
        data->target = str_to_int(target_parts->at(0ll));

        auto _expr1 = parts;
        for (int parts__it1__idx = 0; parts__it1__idx < _expr1->size(); parts__it1__idx++) {
            std::string parts__it1 = (*_expr1)[parts__it1__idx];

            if (parts__it1__idx == 0ll) {
                continue;
            };
            push(data->values, str_to_int(parts__it1));
        }
        push(datas, data);
    }
    return datas;
}

long long concat(long long a, long long b) {
    std::string s;
    s = int_to_str(a) + int_to_str(b);
    return str_to_int(s);
}

std::string try_data(Data* data, long long total, long long value_index, std::string exp) {
    long long next_value;
    std::string next_exp;
    std::string s;

    if (total < 0ll) {
        err("<0"s);
    };

    if (total == data->target && value_index == len(data->values)) {
        return exp;
    };

    if (total > data->target) {
        return ""s;
    };

    if (value_index >= len(data->values)) {
        return ""s;
    };
    next_value = data->values->at(value_index);
    next_exp = exp + int_to_str(next_value);
    s = try_data(data, concat(total, next_value), value_index + 1ll, next_exp);

    if (s != ""s) {
        return s;
    };
    next_exp = exp + "*"s + int_to_str(next_value);
    s = try_data(data, total*next_value, value_index + 1ll, next_exp);

    if (s != ""s) {
        return s;
    };
    next_exp = "("s + exp + " + "s + int_to_str(next_value) + ")"s;
    s = try_data(data, total + next_value, value_index + 1ll, next_exp);

    if (s != ""s) {
        return s;
    };
    return ""s;
}

long long is_data_valid(Data* data) {
    long long first_value;
    std::string s;
    std::string s1;
    first_value = data->values->at(0ll);
    s = try_data(data, first_value, 1ll, int_to_str(first_value));

    if (s != ""s) {
        s1 = int_to_str(data->target) + ";"s + s;
        printf("%s\n", (s1).c_str());
        return 1ll;
    };
    return 0ll;
}

void run() {
    std::vector<std::string>* lines;
    std::vector<Data*>* datas = new std::vector<Data*>();
    long long total;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    datas = lines_to_datas(lines);
    total = 0ll;

    auto _expr0 = datas;
    for (int datas__it0__idx = 0; datas__it0__idx < _expr0->size(); datas__it0__idx++) {
        Data* datas__it0 = (*_expr0)[datas__it0__idx];

        if (is_data_valid(datas__it0)) {
            total = total + datas__it0->target;
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
