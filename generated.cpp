#include "prelude.cpp"

/// prelude template <typename T>i64 array_size(shared_vector<T> a);
/// prelude template <typename T>void array_push(shared_vector<T> a, T element);
/// prelude template <typename T>T array_pop(shared_vector<T> a);
/// prelude template <typename T>void array_quick_sort(shared_vector<T> a);
/// prelude template <typename T>void array_push_front(shared_vector<T> a, T element);
/// prelude template <typename T>T array_pop_front(shared_vector<T> a, T element);
/// prelude template <typename T>T array_last(shared_vector<T> a);
/// prelude template <typename T>void array_set_size(shared_vector<T> a, i64 new_size);
/// prelude template <typename T>i64 array_contains(shared_vector<T> a, T value);
/// prelude template <typename T>void array_remove_at(shared_vector<T> a, i64 index);
/// prelude template <typename T>void array_remove(shared_vector<T> a, T value);
/// prelude template <typename T>i64 array_index_of(shared_vector<T> a, T value);
/// prelude template <typename T>shared_vector<T> array_slice(shared_vector<T> a, i64 from, i64 to);
/// prelude template <typename K, typename V>void map_add(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>V map_get_value(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_add_or_update(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>i64 map_has_key(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_remove(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>i64 map_size(shared_map<K, V> map);
/// prelude template <typename K, typename V>shared_vector<K> map_keys(shared_map<K, V> map);
/// prelude shared_vector<std::string> string_split(std::string s, std::string by);
/// prelude i64 string_to_integer(std::string s);
/// prelude shared_vector<std::string> string_to_chars(std::string s);
/// prelude i64 string_contains(std::string s, std::string subs);
/// prelude i64 string_size(std::string s);
/// prelude std::string string_remove(std::string s, std::string r);
/// prelude std::string string_array_join(shared_vector<std::string> a, std::string delimiter);
/// prelude std::string integer_to_string(i64 i);
/// prelude void error(std::string s);
/// prelude std::string read_line();
/// prelude shared_vector<std::string> read_string_lines_from_file(std::string filename);
//// prelude end
void run() {
    shared_vector<i64> a;
    a = create_shared_vector<i64>();
    array_push(a, 5ll);
    array_push(a, 3ll);
    //// array_push(a, 4)
    //// array_push(a, 1)
    //// array_push(a, 8)
    //// array_push(a, 1)
    array_quick_sort(a);

    auto __expr0 = a;
    for (i64 a__it0__idx = 0; a__it0__idx < (i64)__expr0->size(); a__it0__idx++) {
        i64 a__it0 = (*__expr0)[a__it0__idx];
        printf("%lld\n", (i64)(a__it0));
    }
    ////error("123")
}


