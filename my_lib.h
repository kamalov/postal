#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> 
#include <fstream>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <functional>
using namespace std::literals;

/// hash utils
template <typename T>
struct UniversalHash {
    std::size_t operator()(const T obj) const {
        if constexpr (std::is_pointer_v<T>) {
            using TValue = std::remove_pointer_t<T>;
            std::hash<TValue> hasher;
            //std::cout << hasher(*obj) << std::endl;
            return hasher(*obj);
        }
        else {
            std::hash<T> hasher;
            return hasher(obj);
        }
    }
};

template<typename T>
struct UniversalEquals {
    bool operator()(const T l, const T r) const {
        if constexpr (std::is_pointer_v<T>) {
            return *l == *r;
        }
        else {
            return l == r;
        }
    }
};

template<typename K,typename V>
using universal_hashmap = std::unordered_map<K, V, UniversalHash<K>, UniversalEquals<K>>;

template <typename K, typename V>
void add(universal_hashmap<K, V>* hashmap, K key, V value) {
    K new_key = key;
    
    if constexpr (std::is_pointer_v<K>) {
        using KV = std::remove_pointer_t<K>;
        new_key = new KV();
        std::memcpy(new_key, key, sizeof(KV));
    }

    auto result = hashmap->insert(std::make_pair(new_key, value));
    if (!result.second) {
        throw "Key already exists"s;
    }
}

template <typename K, typename V>
long long has(universal_hashmap<K, V>* h, K key) {
    return h->find(key) != h->end();
}

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
T pop(std::vector<T>* a) {
    T last = a->back();
    a->pop_back();
    return last;
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
    return std::to_string(i);
}

long long str_contains(std::string s, std::string subs) {
    return s.find(subs) != std::string::npos;
}

long long str_len(std::string s) {
    return s.length();
}

/// misc utils
[[noreturn]] void err(std::string s) {
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
