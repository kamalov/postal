#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

std::vector<std::string> read_lines(std::string filename);
std::vector<std::string> split_str(std::string s, std::string by);
long long str_to_int(std::string s);
std::vector<long long> sort_int_array(std::vector<long long> a);
long long abs_int(long long a, long long b) {

    if (a>b) {
        return a - b;
    }
    else if (b>a) {
        return b - a;
    };
    return 0;
}

void run() {
    std::vector<std::string> lines;
    std::vector<long long> left;
    std::vector<long long> right;
    std::vector<std::string> tokens;
    long long sum;
    long long lvalue;
    long long count;
    lines = read_lines("D:/src/postal/aoc2024/input.txt"s);

    for (int lines__it0__idx = 0; lines__it0__idx < lines.size(); lines__it0__idx++) {
        std::string lines__it0 = lines[lines__it0__idx];
        tokens = split_str(lines__it0, " "s);
        left.push_back(str_to_int(tokens[0]));
        right.push_back(str_to_int(tokens[1]));
    }
    left = sort_int_array(left);
    right = sort_int_array(right);

    for (int left__it1__idx = 0; left__it1__idx < left.size(); left__it1__idx++) {
        long long left__it1 = left[left__it1__idx];
        printf("%lld\n", static_cast<long long>(left__it1));
    }

    for (int right__it2__idx = 0; right__it2__idx < right.size(); right__it2__idx++) {
        long long right__it2 = right[right__it2__idx];
        printf("%lld\n", static_cast<long long>(right__it2));
    }
    sum = 0;

    for (int left__it3__idx = 0; left__it3__idx < left.size(); left__it3__idx++) {
        long long left__it3 = left[left__it3__idx];
        lvalue = left__it3;
        count = 0;

        for (int right__it4__idx = 0; right__it4__idx < right.size(); right__it4__idx++) {
            long long right__it4 = right[right__it4__idx];

            if (lvalue == right__it4) {
                count = count + 1;
            };
        }
        lvalue = lvalue*count;
        sum = sum + lvalue;
    }
    printf("\n");
    printf("%lld\n", static_cast<long long>(sum));
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
