#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std::literals;

/// lib

std::vector<std::string> read_lines(std::string filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw "invalid file '"s + filename + "'";
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();
    return lines;
}

/// GENERATED CODE
%GENERATED_CODE%

/// main

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
