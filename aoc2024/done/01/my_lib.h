#include <stdio.h>
#include <windows.h>
#include <vector>
#include <algorithm> 
#include <string>
#include <fstream>

using namespace std::literals;

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

//std::vector<std::string> str_split(const std::string& s, const std::string& delimiter) {
std::vector<std::string> split_str(std::string s, std::string delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

long long str_to_int(std::string s) {
    return std::stoll(s);
}

std::vector<long long> sort_int_array(std::vector<long long> a) {
    std::sort(a.begin(), a.end());
    return a;
}

