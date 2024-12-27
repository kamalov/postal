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
using i64 = long long;

void run();

int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 10);

    try {
        run();
    }
    catch (const std::string& ex) {
        SetConsoleTextAttribute(hConsole, 12);
        printf("Exception: %s\n", ex.c_str());
        return -1;
    }
    catch (const std::exception& e) {
        SetConsoleTextAttribute(hConsole, 12);
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        SetConsoleTextAttribute(hConsole, 12);
        std::cerr << "Unknown exception" << std::endl;
        return -1;
    }

    SetConsoleTextAttribute(hConsole, 7);
    return 0;
}

/// for postal built-in range 
std::vector<i64>* create_range(i64 from, i64 to) {
    std::vector<i64>* a = new std::vector<i64>();
    int size = to - from + 1;
    if (size > 0) {
        a->resize(size);
        for (int i = 0; i < size; i++) {
            (*a)[i] = from + i;
        }
    }
    return a;
}

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
void map_add(universal_hashmap<K, V>* hashmap, K key, V value) {
    K new_key = key;
    
    if constexpr (std::is_pointer_v<K>) {
        using KV = std::remove_pointer_t<K>;
        new_key = new KV();
        std::memcpy(new_key, key, sizeof(KV));
    }

    auto result = hashmap->insert(std::make_pair(new_key, value));
    if (!result.second) {
        throw "Error adding to hashmap: key already exist"s;
    }
}

template <typename K, typename V>
void map_add_or_update(universal_hashmap<K, V>* hashmap, K key, V value) {
    K new_key = key;
    
    if constexpr (std::is_pointer_v<K>) {
        using KV = std::remove_pointer_t<K>;
        new_key = new KV();
        std::memcpy(new_key, key, sizeof(KV));
    }

    (*hashmap)[new_key] = value;
}

template <typename K, typename V>
i64 map_has_key(universal_hashmap<K, V>* h, K key) {
    return h->find(key) != h->end();
}

template <typename K, typename V>
V map_get_value(universal_hashmap<K, V>* h, K key) {
    auto it = h->find(key);
    if (it != h->end()) {
        return it->second;
    }
    throw "Error getting value from hashmap: key not found"s;
}

template <typename K, typename V>
void map_remove(universal_hashmap<K, V>* h, K key) {
    if (h->find(key) == h->end()) {
        throw "Error removing value from hashmap: key not found"s;
    }
    h->erase(key);
}

template <typename K, typename V>
i64 map_len(universal_hashmap<K, V>* h) {
    return h->size();
}

template <typename K, typename V>
std::vector<K>* map_keys(universal_hashmap<K, V>* h) {
    std::vector<K>* keys = new std::vector<K>();
    for (const auto& pair : *h) {
        keys->push_back(pair.first);
    }
    return keys;
}

/// dyn array utils
template <typename T>
i64 len(std::vector<T>* a) { 
    return a->size();
}

template <typename T>
void push(std::vector<T>* a, T elem) {
    a->push_back(elem);
}

template <typename T>
T pop(std::vector<T>* a) {
    T last = a->back();
    a->pop_back();
    return last;
}

template <typename T>
void arr_set_len(std::vector<T>* a, i64 new_len) {
    a->resize(new_len);
}

template <typename T>
void arr_sort(std::vector<T>* a) {
    std::sort(a->begin(), a->end());
}

template <typename T>
i64 arr_contains(std::vector<T>* a, T value) {
    if constexpr (std::is_pointer<T>::value) {
        return std::find_if(a->begin(), a->end(), [value](T item) { return *item == *value; }) != a->end();
    } else {
	    return std::find(a->begin(), a->end(), value) != a->end();
    }
}

template <typename T>
void arr_remove(std::vector<T>* a, T value) {
    if constexpr (std::is_pointer<T>::value) {
        a->erase(std::remove_if(a->begin(), a->end(), [value](T item){ return *item == *value; }), a->end());
    } else {
        a->erase(std::remove_if(a->begin(), a->end(), [value](T item){ return item == value; }), a->end());
    }
}

template <typename T>
void arr_remove_at(std::vector<T>* a, i64 index) {
    a->erase(a->begin() + index);
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

i64 str_to_int(std::string s) {
    return std::stoll(s);
}

std::string int_to_str(i64 i) {
    return std::to_string(i);
}

i64 str_contains(std::string s, std::string subs) {
    return s.find(subs) != std::string::npos;
}

std::string str_remove(std::string s, std::string r) {
    std::string result = s;
    size_t start_pos = 0;
    while ((start_pos = result.find(r)) != std::string::npos) {
        result.erase(start_pos, r.length());
    }
    return result;
}

i64 str_len(std::string s) {
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
