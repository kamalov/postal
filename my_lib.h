#include <stdio.h>
#include <windows.h>
#include <vector>
#include <algorithm> 
#include <string>
#include <fstream>

using namespace std::literals;



/// dyn array utils
template <typename T>
long long len(std::vector<T>* a) { 
    return a->size();
}

template <typename T>
void set_size(std::vector<T>* a, long long new_size) {
    a->resize(new_size);
}

template <typename T>
void push(std::vector<T>* a, T elem) {
    a->push_back(elem);
}

template <typename T>
void set_value(std::vector<T>* a, long long index, T elem) {
    (*a)[index] = elem;
}

template <typename T>
void sort(std::vector<T>* a) {
    std::sort(a->begin(), a->end());
}



/// range utils
std::vector<long long>* create_range(long long from, long long to) {
    std::vector<long long>* a = new std::vector<long long>();    
    int size = to - from + 1;
    if (size > 0) {
	a->resize(size);
	for (int i = 0; i < size; i++) {
	    (*a)[i] = from + i;
        }
    }
    return a;
}



/// string utils
std::vector<std::string>* split_str(std::string s, std::string delimiter) {
    std::vector<std::string>* tokens = new std::vector<std::string>();
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens->push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens->push_back(s);

    return tokens;
}

std::vector<std::string>* str_to_chars(std::string s) {
    std::vector<std::string>* chars = new std::vector<std::string>();
    for (char c : s) {
        chars->push_back(std::string(1, c));
    }
    return chars;
}

long long str_to_int(std::string s) {
    return std::stoll(s);
}

std::string int_to_str(long long i) {
    std::string s = std::to_string(i);
    return s;
}

long long str_contains(std::string s, std::string subs) {
    return s.find(subs) != std::string::npos;
}



/// file utils
std::vector<std::string>* read_lines(std::string filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw "invalid file '"s + filename + "'";
    }
    std::vector<std::string>* lines = new std::vector<std::string>();
    std::string line;
    while (std::getline(infile, line)) {
        lines->push_back(line);
    }
    infile.close();
    return lines;
}