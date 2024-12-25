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
template <typename K, typename V>void add(universal_hashmap<K, V>* hashmap, K key, V value);
template <typename K, typename V>long long has(universal_hashmap<K, V>* hashmap, K key);
//fn get<K, V>(map: #[K, V], key: K) V external
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
    universal_hashmap<A*, long long>* amap;
    A* key;
    universal_hashmap<std::string, long long>* vmap;
    amap = new universal_hashmap<A*, long long>();
    key = new A();
    key->a = 1ll;
    add(amap, key, 1ll);
    printf("%lld\n", static_cast<long long>(has(amap, key)));
    key->a = 2ll;
    printf("%lld\n", static_cast<long long>(has(amap, key)));
    vmap = new universal_hashmap<std::string, long long>();
    add(vmap, "one"s, 1ll);
    printf("%lld\n", static_cast<long long>(has(vmap, "one"s)));
    printf("%lld\n", static_cast<long long>(has(vmap, "two"s)));
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