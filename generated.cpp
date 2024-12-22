#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm> 
#include <fstream>
#include <iostream>
using namespace std::literals;

/// built-in range
std::vector<long long>* create_range(long long from, long long to);

/// generated code
// fn len<T>(a: [T]) int external
template <typename T>void push(std::vector<T>* a, T elem);
// fn array_contains<T>(a: [T], value: T) int external
template <typename T>T array_last(std::vector<T>* a);
long long test() {
    return 1ll;
}

void run() {
    std::vector<std::string>* a;
    std::string b;
    long long c;
    a = new std::vector<std::string>();
    push(a, "one"s);
    push(a, "two"s);
    b = array_last(a);
    c = test();
    printf("%s %lld\n", (b).c_str(), static_cast<long long>(c));
}



/// template code
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

/// lib 

/// for postal built-in range 
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

/// dyn array utils
template <typename T>
long long len(std::vector<T>* a) { 
    return a->size();
}

template <typename T>
void push(std::vector<T>* a, T elem) {
    a->push_back(elem);
}

template <typename T>
void array_set_len(std::vector<T>* a, long long new_len) {
    a->resize(new_len);
}

template <typename T>
void array_sort(std::vector<T>* a) {
    std::sort(a->begin(), a->end());
}

template <typename T>
long long array_contains(std::vector<T>* a, T value) {
    if constexpr (std::is_pointer<T>::value) {
        return std::find_if(a->begin(), a->end(), [value](T item) { return *item == *value; }) != a->end();
    } else {
	return std::find(a->begin(), a->end(), value) != a->end();
    }
}

template <typename T>
void array_remove(std::vector<T>* a, T value) {
    if constexpr (std::is_pointer<T>::value) {
	a->erase(std::remove_if(a->begin(), a->end(), [value](T item){ return *item == *value; }), a->end());
    } else {
	a->erase(std::remove_if(a->begin(), a->end(), [value](T item){ return item == value; }), a->end());
    }
}

template <typename T>
void array_remove_at(std::vector<T>* a, long long index) {
    a->erase(a->begin() + index);
}

template <typename T>
T array_last(std::vector<T>* a) {
    return a->back();
}

/// string utils
std::vector<std::string>* str_split(std::string s, std::string delimiter) {
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

long long str_len(std::string s) {
    return s.length();
}

/// misc utils
[[noreturn]]  void err(std::string s) {
    throw s;
}

std::string read_line_from_console() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::vector<std::string>* read_string_lines_from_file(std::string filename) {
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
