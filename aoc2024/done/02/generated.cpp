#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

std::vector<std::string> read_lines(std::string filename);
std::vector<std::string> split_str(std::string s, std::string by);
long long str_to_int(std::string s);
std::string int_to_str(long long i);
std::vector<long long> sort_int_array(std::vector<long long> a);
long long int_array_len(std::vector<long long> a);
long long abs_int(long long a) {

    if (a < 0) {
        return 0 - a;
    };
    return a;
}

struct Report {
    std::vector<long long> values;
};

long long is_valid(std::vector<long long> vals) {
    long long prev_d;
    long long i;
    long long prev;
    long long v;
    long long d;
    prev_d = vals[0] - vals[1];

    if (prev_d == 0 || abs_int(prev_d) > 3) {
        return 0;
    };
    i = 2;
    prev = vals[1];

    for (;;) {
        v = vals[i];
        d = prev - v;

        if (d == 0 || abs_int(d) > 3) {
            return 0;
        };

        if (prev_d < 0 && d > 0 || prev_d > 0 && d < 0) {
            return 0;
        };
        prev_d = d;
        prev = v;
        i = i + 1;

        if (i >= int_array_len(vals)) {
            break;
        };
    }
    return 1;
}

long long is_any_valid(std::vector<long long> vals) {
    Report r;
    long long skip_index;

    for (int vals__it0__idx = 0; vals__it0__idx < vals.size(); vals__it0__idx++) {
        long long vals__it0 = vals[vals__it0__idx];
        r = Report {};
        skip_index = vals__it0__idx;

        for (int vals__it1__idx = 0; vals__it1__idx < vals.size(); vals__it1__idx++) {
            long long vals__it1 = vals[vals__it1__idx];

            if (vals__it1__idx != skip_index) {
                r.values.push_back(vals__it1);
            };
        }

        if (is_valid(r.values)) {
            return 1;
        };
    }
    return 0;
}

void run() {
    std::vector<std::string> lines;
    std::vector<Report> reports;
    std::vector<std::string> tokens;
    Report report;
    long long valid_count;
    std::string s;
    std::vector<long long> values;
    lines = read_lines("D:/src/postal/aoc2024/input.txt"s);

    for (int lines__it0__idx = 0; lines__it0__idx < lines.size(); lines__it0__idx++) {
        std::string lines__it0 = lines[lines__it0__idx];
        tokens = split_str(lines__it0, " "s);
        report = Report {};

        for (int tokens__it1__idx = 0; tokens__it1__idx < tokens.size(); tokens__it1__idx++) {
            std::string tokens__it1 = tokens[tokens__it1__idx];
            report.values.push_back(str_to_int(tokens__it1));
        }
        reports.push_back(report);
    }
    valid_count = 0;

    for (int reports__it2__idx = 0; reports__it2__idx < reports.size(); reports__it2__idx++) {
        Report reports__it2 = reports[reports__it2__idx];
        s = ""s;
        values = reports__it2.values;

        for (int values__it3__idx = 0; values__it3__idx < values.size(); values__it3__idx++) {
            long long values__it3 = values[values__it3__idx];
            s = s + " -> "s + int_to_str(values__it3);
        }
        printf("%s\n", (s).c_str());

        if (is_any_valid(values)) {
            valid_count = valid_count + 1;
            printf("%s\n", ("valid"s).c_str());
        }
        else {
            printf("%s\n", ("invalid"s).c_str());
        };
    }
    printf("%lld\n", static_cast<long long>(valid_count));
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
