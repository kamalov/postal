#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> 
#include <fstream>
#include <iostream>
#include "my_lib.h"

using namespace std::literals;

/// generated code
/// lib array utils
template <typename T>long long len(std::vector<T>* a);
template <typename T>void push(std::vector<T>* a, T elem);
template <typename T>T pop(std::vector<T>* a);
template <typename T>void array_set_len(std::vector<T>* a, long long new_len);
template <typename T>void array_sort(std::vector<T>* a);
template <typename T>long long array_contains(std::vector<T>* a, T value);
template <typename T>void array_remove_at(std::vector<T>* a, long long index);
template <typename T>void array_remove(std::vector<T>* a, T value);
/// lib hashmap utils
template <typename K, typename V>void hashmap_add(universal_hashmap<K, V>* hashmap, K key, V value);
template <typename K, typename V>void hashmap_add_or_update(universal_hashmap<K, V>* hashmap, K key, V value);
template <typename K, typename V>long long hashmap_has_key(universal_hashmap<K, V>* hashmap, K key);
template <typename K, typename V>V hashmap_get_value(universal_hashmap<K, V>* hashmap, K key);
/// lib string utils
std::vector<std::string>* str_split(std::string s, std::string by);
long long str_to_int(std::string s);
std::vector<std::string>* str_to_chars(std::string s);
long long str_contains(std::string s, std::string subs);
long long str_len(std::string s);
std::string int_to_str(long long i);
/// lib other utils
void err(std::string s);
std::string read_line_from_console();
std::vector<std::string>* read_string_lines_from_file(std::string filename);
struct A {
    long long a;
    std::string b;
    friend bool operator==(const A& l, const A& r) {
        return (l.a == r.a) && (l.b == r.b);
    }
};

namespace std {
    template<>
    struct hash<A> {
        std::size_t operator()(const A& rec) const {
            size_t h = 0;
            h ^= std::hash<long long>{}(rec.a) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<std::string>{}(rec.b) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

void run() {
    universal_hashmap<A*, std::string>* amap;
    A* key;
    universal_hashmap<long long, long long>* vmap;
    long long b;
    amap = new universal_hashmap<A*, std::string>();
    key = new A();
    key->a = 1ll;
    hashmap_add(amap, key, "one"s);
    printf("%s\n", (hashmap_get_value(amap, key)).c_str());
    hashmap_add_or_update(amap, key, "two"s);
    key->b = "one"s;
    printf("%lld\n", static_cast<long long>(hashmap_has_key(amap, key)));
    vmap = new universal_hashmap<long long, long long>();
    hashmap_add(vmap, 1ll, 2ll);
    //log(has(vmap, 'one'))
    b = hashmap_get_value(vmap, 1ll);
    printf("%lld\n", static_cast<long long>(b));
    //log(has(vmap, 'two'))
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
        printf("Exception: %s\n", ex.c_str());
        return -1;
    } catch (const std::exception& e) {
        SetConsoleTextAttribute(hConsole, 12);
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        SetConsoleTextAttribute(hConsole, 12);
        std::cerr << "Unknown exception" << std::endl;
        return -1;
    }
    
    SetConsoleTextAttribute(hConsole, 7);
    return 0;
}