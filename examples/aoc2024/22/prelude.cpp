#include <stdio.h>
#include <stdlib.h>
#include <vector> // todo, make local vector
#include <unordered_map> // todo, make local hashmap
#include <string> // todo, make local strings

using namespace std::literals;

/// type aliases
using i64 = long long;



#if 0 // todo: replace std::string with this
#include <stdlib.h>
#include <string.h>

/// String
class String {
private:
    size_t size = 0;
    char* buffer = 0;

    void create_or_update_from_char_pointer(char* from) {
        //printf("create_or_update_from_char_pointer '%s'\n", from);
        if (buffer) {
            free(buffer);
            buffer = 0;
        }
        size = strlen(from);
        buffer = (char*)malloc((size + 1) * sizeof(char));
        strcpy_s(buffer, size + 1, from);
    }

public:
    String() {
        //printf("DEFAULT\n");
        create_or_update_from_char_pointer((char*)"");
    }

    String(const char from[]) {
        //printf("CREATE FROM CHAR\n");
        create_or_update_from_char_pointer((char*)from);
    }

    String(const String& other) {
        //printf("COPY\n");
        create_or_update_from_char_pointer(other.buffer);
    }

    void operator=(String other) {
        //printf("ASSIGNMENT\n");
        create_or_update_from_char_pointer(other.buffer);
    }

    String operator+(String other) {
        //printf("PLUS\n");
        size_t compound_size = size + other.size;
        char* tmp_buffer = (char*)malloc((compound_size + 1) * sizeof(char));
        strcpy_s(tmp_buffer, size + 1, buffer);
        strcpy_s(tmp_buffer + size, other.size + 1, other.buffer);
        String result = String(tmp_buffer);
        free(tmp_buffer);
        return result;
    }

    ~String() {
        //printf("destructuring '%s'\n", buffer);
        free(buffer);
    }

    char* c_str() {
        return buffer;
    }
};

#define _throw(msg) throw String(msg) + String(" at ") + String(__FILE__); //__LINE__);
#endif



#define _throw(msg) throw std::string{} + msg + " at '" + __FILE__ +"', line " + std::to_string(__LINE__);



/// Shared Pointers
i64 shared_pointer_debug_counter = 0;

template <typename T> struct Shared_Pointer {
    unsigned int* counter_ptr = nullptr;
    T* ptr = nullptr;

    Shared_Pointer(T* from_ptr = nullptr) {
        shared_pointer_debug_counter++;
        ptr = from_ptr;
        counter_ptr = new unsigned int(1);
    }

    Shared_Pointer(const Shared_Pointer& other) {
        if (ptr || counter_ptr) {
            _throw("improper shared copy constructor call\n");
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
            shared_pointer_debug_counter--;
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
            _throw("npe");
        }
        return *ptr;
    }
    
    T* operator->() const { 
        if (ptr == nullptr) {
            _throw("npe");
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

template <typename T>             using shared_pointer = Shared_Pointer<T>;
template <typename T>             using shared_vector = Shared_Pointer<std::vector<T>>;
template <typename T>             constexpr auto create_shared_pointer = &Shared_Pointer<T>::create;
template <typename T>             constexpr auto create_shared_vector = &Shared_Pointer<std::vector<T>>::create;

template <typename K, typename V> using shared_map = Shared_Pointer<std::unordered_map<K, V> >;
template <typename K, typename V> constexpr auto create_shared_map = &Shared_Pointer<std::unordered_map<K, V>>::create;



/// Hashmap Utils
template <typename K, typename V> i64 map_has_key(shared_map<K, V> h, K k) { return h->find(k) != h->end(); }
template <typename K, typename V> i64 map_size(shared_map<K, V> h) { return h->size(); }

template <typename K, typename V>
void map_add(shared_map<K, V> hashmap, K key, V value) {
    auto result = hashmap->insert(std::make_pair(key, value));
    if (!result.second) _throw("Error adding to hashmap: key already exist");
}

template <typename K, typename V>
void map_add(shared_map<shared_pointer<K>, V> hashmap, shared_pointer<K> key, V value) {
    // cloning shared key data
    K* new_key_data = new K();
    std::memcpy(new_key_data, key.ptr, sizeof(K));
    shared_pointer<K> new_key(new_key_data);
    auto result = hashmap->insert(std::make_pair(new_key, value));
    if (!result.second) _throw("Error adding to hashmap: key already exist");
}

template <typename K, typename V>
V map_get_value(shared_map<K, V> hashmap, K key) {
    auto it = hashmap->find(key);
    if (it != hashmap->end()) return it->second;
    _throw("Error getting value from hashmap : key not found");
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
        _throw("Error removing value from hashmap: key not found");
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



/// Dyn Array Utils
template <typename T> i64 array_size(shared_vector<T> a) { return a->size(); }
template <typename T> i64 array_contains(shared_vector<T> a, T value) { return std::find(a->begin(), a->end(), value) != a->end(); }
template <typename T> void array_push(shared_vector<T> a, T elem) { a->push_back(elem); }
template <typename T> void array_push_front(shared_vector<T> a, T elem) { a->insert(a->begin(), elem); }
template <typename T> void array_set_size(shared_vector<T> a, i64 new_len) { a->resize(new_len); }
template <typename T> void array_remove(shared_vector<T> a, T value) { a->erase(std::remove_if(a->begin(), a->end(), [value](T item){ return item == value; }), a->end()); }
template <typename T> void array_remove_at(shared_vector<T> a, i64 index) { a->erase(a->begin() + index); }
template <typename T> T array_last(shared_vector<T> a) { return a->back(); }

template <typename T>
T array_pop(shared_vector<T> a) {
    T last = a->back();
    a->pop_back();
    return last;
}

template <typename T>
T array_pop_front(shared_vector<T> a) {
    T first = a->at(0);
    a->erase(a->begin());
    return first;
}

template <typename T>
i64 array_index_of(shared_vector<T> a, T value) {
    auto it = std::find(a->begin(), a->end(), value);
    if (it != a->end()) {
        return std::distance(a->begin(), it);
    }
    return -1;
}

template <typename T>
shared_vector<T> array_slice(shared_vector<T> a, i64 from_index, i64 to_index) {
    auto clamp = [](i64 v, i64 min, i64 max) {
        if (v < min) return min;
        if (v > max) return max;
        return v;
    };

    from_index = clamp(from_index, 0, a->size() - 1);
    to_index = clamp(to_index, 0, a->size() - 1);
    if (from_index > to_index) {
        return create_shared_vector<T>();
    }
    return shared_vector<T>(new std::vector<T>(a->begin() + from_index, a->begin() + to_index + 1));
}

template <typename T>
void array_quick_sort_helper(shared_vector<T> &array, i64 begin_index, i64 end_index) {
    if (begin_index >= end_index) return;
    T some_value = array->at((begin_index + end_index)/2);
    i64 i = begin_index;
    i64 j = end_index;
    while (true) {
        while (array->at(i) < some_value) {
            i++;
        }

        while (array->at(j) > some_value) {
            j--;
        }

        if (i >= j) break;

        T tmp = array->at(i);
        array->at(i) = array->at(j);
        array->at(j) = tmp;
        i++;
        j--;
    }

    array_quick_sort_helper(array, begin_index, j);
    array_quick_sort_helper(array, j + 1, end_index);
}

template <typename T> 
void array_quick_sort(shared_vector<T> array) {
    array_quick_sort_helper(array, 0, array->size() - 1);
}   



/// String Utils
i64 string_to_integer(std::string s) { return std::stoll(s); }
i64 string_contains(std::string s, std::string subs) { return s.find(subs) != std::string::npos; }
i64 string_size(std::string s) { return s.length(); }
std::string integer_to_string(i64 i) { return std::to_string(i); }

shared_vector<std::string> string_split(std::string s, std::string delimiter) {
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

shared_vector<std::string> string_to_chars(std::string s) {
    auto chars = create_shared_vector<std::string>();
    for (char c : s) chars->push_back(std::string(1, c));
    return chars;
}

std::string string_remove(std::string s, std::string r) {
    std::string result = s;
    size_t start_pos = 0;
    while ((start_pos = result.find(r)) != std::string::npos) {
        result.erase(start_pos, r.length());
    }
    return result;
}

std::string string_array_join(shared_vector<std::string> a, std::string delimiter) {
    auto s = ""s;
    for (std::size_t i = 0; i != a->size(); i++) {
        if (i > 0) s += delimiter;
        s += (*a)[i];
    }
    return s;
}



/// Misc Utils
[[noreturn]] void error(std::string s) {
    _throw(s);
}

std::string read_line() {
    char buffer[4096];
    std::string line;
    if (fgets(buffer, sizeof(buffer), stdin)) {
        line = std::string(buffer);
        if (line.back() == '\n') {
            line.pop_back();
        }
    }

    return line;
}

shared_vector<std::string> read_string_lines_from_file(std::string filename) {
    FILE* file;
    char buffer[4096];

    fopen_s(&file, filename.c_str(), "r");
    if (file == NULL) {
        throw "Error reading file '"s + filename + "'";
    }
    
    std::vector<std::string>* lines = new std::vector<std::string>();

    while (fgets(buffer, sizeof(buffer), file)) {
        std::string line = std::string(buffer);
        if (line.back() == '\n') {
            line.pop_back();
        }
        lines->push_back(line);
    }

    fclose(file);

    return lines;
}

// for built-in range 
shared_vector<std::size_t> create_range(i64 from, i64 to) {
    std::vector<std::size_t>* a = new std::vector<std::size_t>();
    i64 diff = (to - from + 1);
    if (from >= 0 && to >= 0 && diff > 0) {
        auto size = static_cast<std::size_t>(diff);
        a->resize(size);
        for (std::size_t i = 0; i != size; i++) {
            (*a)[i] = (std::size_t)(from + i);
        }
    }
    return a;
}



/// Main
void run();

int main() {
    //windows.h: todo
    //HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    //SetConsoleTextAttribute(hConsole, 10);

    //String a = String("test_");
    //String b = String("value");
    //String c;
    //c = String("test_") + String("value");

    try {
        run();

        if (shared_pointer_debug_counter) {
            printf("\nshared_pointer_debug_counter = %lld\n", shared_pointer_debug_counter);
            throw "shared pointer error";
        }
    }
    catch (std::string& string_exception) {
        //SetConsoleTextAttribute(hConsole, 12);
        printf("Error: %s\n", string_exception.c_str());
        return -1;
    }
    catch (const std::exception& e) {
        //SetConsoleTextAttribute(hConsole, 12);
        printf("Exception caught: %s\n", e.what());
        return -1;
    }
    catch (...) {
        //SetConsoleTextAttribute(hConsole, 12);
        printf("Unknown exception\n");
        return -1;
    }

    //SetConsoleTextAttribute(hConsole, 7);
    return 0;
}
