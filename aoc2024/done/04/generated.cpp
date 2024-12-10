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
struct Row {
    std::vector<std::string>* cols;
};

std::string get_cell_value(std::vector<Row*>* rows, long long row_index, long long col_index) {
    long long rows_count;
    long long cols_count;
    rows_count = rows->size();
    cols_count = rows->at(0)->cols->size();

    if (row_index < 0 || col_index < 0 || row_index >= rows_count || col_index >= cols_count) {
        return ""s;
    };
    return rows->at(row_index)->cols->at(col_index);
}

long long try_xmas(std::vector<Row*>* rows, long long row_index, long long col_index, long long dx, long long dy) {
    long long x;
    long long m;
    long long a;
    long long s;
    x = get_cell_value(rows, row_index, col_index) == "X"s;
    m = get_cell_value(rows, row_index + dy, col_index + dx) == "M"s;
    a = get_cell_value(rows, row_index + dy*2, col_index + dx*2) == "A"s;
    s = get_cell_value(rows, row_index + dy*3, col_index + dx*3) == "S"s;

    if (x && m && a && s) {
        return 1;
    };
    return 0;
}

long long is_ms(std::string a, std::string b) {

    if (a == "M"s && b == "S"s || a == "S"s && b == "M"s) {
        return 1;
    };
    return 0;
}

long long total_xmas_from_cell(std::vector<Row*>* rows, long long row_index, long long col_index) {
    std::string c;
    std::string top_left;
    std::string bottom_right;
    std::string top_right;
    std::string bottom_left;
    c = get_cell_value(rows, row_index, col_index);

    if (c != "A"s) {
        return 0;
    };
    top_left = get_cell_value(rows, row_index - 1, col_index - 1);
    bottom_right = get_cell_value(rows, row_index + 1, col_index + 1);
    top_right = get_cell_value(rows, row_index + 1, col_index - 1);
    bottom_left = get_cell_value(rows, row_index - 1, col_index + 1);

    if (is_ms(top_left, bottom_right) && is_ms(top_right, bottom_left)) {
        return 1;
    };
    return 0;
}

void run() {
    std::vector<std::string>* lines;
    std::vector<Row*>* rows = new std::vector<Row*>();
    Row* row = new Row();
    long long total;
    long long row_index;
    long long col_index;
    lines = read_lines("D:/src/postal/aoc2024/input.txt"s);
    rows = new std::vector<Row*>();

    for (int lines__it0__idx = 0; lines__it0__idx < lines->size(); lines__it0__idx++) {
        std::string lines__it0 = lines->at(lines__it0__idx);
        row = new Row();
        row->cols = str_to_chars(lines__it0);
        rows->push_back(row);
    }
    total = 0;

    for (int rows__it1__idx = 0; rows__it1__idx < rows->size(); rows__it1__idx++) {
        Row* rows__it1 = rows->at(rows__it1__idx);
        row_index = rows__it1__idx;

        for (int exp__it2__idx = 0; exp__it2__idx < row->cols->size(); exp__it2__idx++) {
            std::string exp__it2 = row->cols->at(exp__it2__idx);
            col_index = exp__it2__idx;
            total = total + total_xmas_from_cell(rows, row_index, col_index);
        }
    }
    printf("%lld\n", static_cast<long long>(total));
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
