#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> 
#include <tuple>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <type_traits>

using namespace std::literals;
using i64 = long long;
                                   
i64 sp_counter = 0;

template <typename T> struct Shared_Pointer {
    unsigned int* counter_ptr = nullptr;
    T* ptr = nullptr;

    Shared_Pointer(T* from_ptr = nullptr) {
        ptr = from_ptr;
        counter_ptr = new unsigned int(1);
    }

    Shared_Pointer(const Shared_Pointer& other) {
        if (ptr || counter_ptr) {
            throw "improper shared copy constructor call\n"s;
        }

        ptr = other.ptr;
        counter_ptr = other.counter_ptr;
        (*counter_ptr)++;
    }

    ~Shared_Pointer() {
       	//printf("sp Destructor %p %lld\n", ptr, sp_counter);
        decrement_counter();
    }

    void decrement_counter() {
        (*counter_ptr)--;
        if (*counter_ptr == 0) {
            //printf("sp Destroy %lld\n", sp_counter--);
            delete counter_ptr;
            delete ptr;
        }
    }

    void operator=(Shared_Pointer other) {
	    //printf("operator=\n");
        if (ptr != other.ptr) {
            decrement_counter();
            ptr = other.ptr;
            if (ptr) {
                counter_ptr = other.counter_ptr;
                (*counter_ptr)++;
            }
        }
    }

    T& operator*() const { 
        if (ptr == nullptr) {
            throw "npe"s;
        }
        return *ptr;
    }
    
    T* operator->() const { 
        if (ptr == nullptr) {
            throw "npe"s;
        }
        return ptr; 
    }
    
    bool operator==(const Shared_Pointer& other) const {
        return (*ptr == *other.ptr);
    }
    
    static Shared_Pointer create() {
        Shared_Pointer sp(new T());
        return sp;
    }
};

template<class T>
struct std::hash<Shared_Pointer<T>> {
    std::size_t operator()(const Shared_Pointer<T>& sp) const {
        std::hash<T> hasher;
        return hasher(*sp);
    }
};

template <typename T> 
using shared_pointer = Shared_Pointer<T>;
template <typename T> 
using shared_vector = Shared_Pointer<std::vector<T>>;
template <typename K, typename V> 
using shared_map = Shared_Pointer<std::unordered_map<K, V> >;
template <typename T> 
constexpr auto create_shared_pointer = &Shared_Pointer<T>::create;
template <typename T> 
constexpr auto create_shared_vector = &Shared_Pointer<std::vector<T>>::create;
template <typename K, typename V> 
const auto create_shared_map = &Shared_Pointer<std::unordered_map<K, V>>::create;

/// hash utils
template <typename K, typename V> i64 map_has_key(shared_map<K, V> h, K k) { return h->find(k) != h->end(); }
template <typename K, typename V> i64 map_len(shared_map<K, V> h) { return h->size(); }

template <typename K, typename V>
void map_add(shared_map<K, V> hashmap, K key, V value) {
    auto result = hashmap->insert(std::make_pair(key, value));
    if (!result.second) throw "Error adding to hashmap: key already exist"s;
}

template <typename K, typename V>
void map_add(shared_map<shared_pointer<K>, V> hashmap, shared_pointer<K> key, V value) {
    // cloning shared key data
    K* new_key_data = new K();
    std::memcpy(new_key_data, key.ptr, sizeof(K));
    shared_pointer<K> new_key(new_key_data);
    auto result = hashmap->insert(std::make_pair(new_key, value));
    if (!result.second) throw "Error adding to hashmap: key already exist"s;
}

template <typename K, typename V>
V map_get_value(shared_map<K, V> hashmap, K key) {
    auto it = hashmap->find(key);
    if (it != hashmap->end()) return it->second;
    throw "Error getting value from hashmap: key not found"s;
}

template <typename K, typename V>
void map_add_or_update(shared_map<K, V> hashmap, K key, V value) {
    (*hashmap)[key] = value;
}

template <typename K, typename V>
void map_add_or_update(shared_map<shared_pointer<K>, V> map, shared_pointer<K> key, V value) {
    if (map.find(key) != map.end()) {
        (*map)[key] = value;
    } else {
        K* new_key_data = new K();
        std::memcpy(new_key_data, key.ptr, sizeof(K));
        shared_pointer<K> new_key(new_key_data);
        (*map)[new_key] = value;
    }
}

template <typename K, typename V>
void map_remove(shared_map<K, V> h, K key) {
    if (h->find(key) == h->end()) {
        throw "Error removing value from hashmap: key not found"s;
    }
    h->erase(key);
}

template <typename K, typename V>
shared_vector<K> map_keys(shared_map<K, V> h) {
    auto keys = create_shared_vector<K>();
    for (const auto& pair : *h) {
        keys->push_back(pair.first);
    }
    return keys;
}



/// dyn array utils
template <typename T> i64 len(shared_vector<T>& a) { return a->size(); }
template <typename T> i64 arr_contains(shared_vector<T>& a, T value) { return std::find(a->begin(), a->end(), value) != a->end(); }
template <typename T> void push(shared_vector<T>& a, T elem) { a->push_back(elem); }
template <typename T> void arr_push_front(shared_vector<T> a, T elem) { a->insert(a->begin(), elem); }
template <typename T> void arr_set_len(shared_vector<T>& a, i64 new_len) { a->resize(new_len); }
template <typename T> void sort(shared_vector<T>& a) { std::sort(a->begin(), a->end()); }
template <typename T> void arr_remove(shared_vector<T>& a, T value) { a->erase(std::remove_if(a->begin(), a->end(), [value](T item){ return item == value; }), a->end()); }
template <typename T> void arr_remove_at(shared_vector<T>& a, i64 index) { a->erase(a->begin() + index); }
template <typename T> T arr_last(shared_vector<T>& a) { return a->back(); }

template <typename T>
T pop(shared_vector<T> a) {
    T last = a->back();
    a->pop_back();
    return last;
}

template <typename T>
T arr_pop_front(shared_vector<T> a) {
    T first = a->at(0);
    a->erase(a->begin());
    return first;
}

template <typename T>
i64 arr_index_of(shared_vector<T> a, T value) {
    auto it = std::find(a->begin(), a->end(), value);
    if (it != a->end()) {
        return std::distance(a->begin(), it);
    }
    return -1;
}

template <typename T>
shared_vector<T> arr_slice(shared_vector<T> a, i64 from_index, i64 to_index) {
    from_index = std::clamp(from_index, (i64)0, (i64)(a->size() - 1));
    to_index = std::clamp(to_index, (i64)0, (i64)(a->size() - 1));
    if (from_index > to_index) {
        return create_shared_vector<T>();
    }
    return shared_vector<T>(new std::vector<T>(a->begin() + from_index, a->begin() + to_index + 1));
}



/// string utils
i64 str_to_int(std::string s) { return std::stoll(s); }
i64 str_contains(std::string s, std::string subs) { return s.find(subs) != std::string::npos; }
i64 str_len(std::string s) { return s.length(); }
std::string int_to_str(i64 i) { return std::to_string(i); }

shared_vector<std::string> str_split(std::string s, std::string delimiter) {
    auto tokens = create_shared_vector<std::string>();
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

shared_vector<std::string> str_to_chars(std::string s) {
    auto chars = create_shared_vector<std::string>();
    for (char c : s) chars->push_back(std::string(1, c));
    return chars;
}

std::string str_remove(std::string s, std::string r) {
    std::string result = s;
    size_t start_pos = 0;
    while ((start_pos = result.find(r)) != std::string::npos) {
        result.erase(start_pos, r.length());
    }
    return result;
}

std::string str_arr_join(shared_vector<std::string> a, std::string delimiter) {
    std::ostringstream ss;
    for (size_t i = 0; i < a->size(); ++i) {
        ss << a->at(i);
        if (i != a->size() - 1) {
            ss << delimiter;
        }
    }
    return ss.str();
}

/// misc utils
[[noreturn]] void err(std::string s) { throw s; }

std::string readln() {
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

// for built-in range 
std::vector<i64>* create_range(i64 from, i64 to) {
    std::vector<i64>* a = new std::vector<i64>();
    auto size = to - from + 1;
    if (size > 0) {
        a->resize(size);
        for (int i = 0; i < size; i++) {
            (*a)[i] = from + i;
        }
    }
    return a;
}



/// main
void run();

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 10);

    try {
        run();
        if (sp_counter) {
            printf("\nsp_counter = %lld\n", sp_counter);
            //throw "shared pointer error"s;
        }
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
