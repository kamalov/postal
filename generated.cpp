#include "prelude.cpp"

/// prelude template <typename T>i64 len(shared_vector<T> a);
/// prelude template <typename T>void push(shared_vector<T> a, T elem);
/// prelude template <typename T>T pop(shared_vector<T> a);
/// prelude template <typename T>void sort(shared_vector<T> a);
/// prelude template <typename T>void arr_push_front(shared_vector<T> a, T elem);
/// prelude template <typename T>T arr_pop_front(shared_vector<T> a, T elem);
/// prelude template <typename T>T arr_last(shared_vector<T> a);
/// prelude template <typename T>void arr_set_len(shared_vector<T> a, i64 new_len);
/// prelude template <typename T>i64 arr_contains(shared_vector<T> a, T value);
/// prelude template <typename T>void arr_remove_at(shared_vector<T> a, i64 index);
/// prelude template <typename T>void arr_remove(shared_vector<T> a, T value);
/// prelude template <typename T>i64 arr_index_of(shared_vector<T> a, T value);
/// prelude template <typename T>shared_vector<T> arr_slice(shared_vector<T> a, i64 from, i64 to);
/// prelude template <typename K, typename V>void map_add(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>V map_get_value(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_add_or_update(shared_map<K, V> map, K key, V value);
/// prelude template <typename K, typename V>i64 map_has_key(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>void map_remove(shared_map<K, V> map, K key);
/// prelude template <typename K, typename V>i64 map_len(shared_map<K, V> map);
/// prelude template <typename K, typename V>shared_vector<K> map_keys(shared_map<K, V> map);
/// prelude shared_vector<std::string> str_split(std::string s, std::string by);
/// prelude i64 str_to_int(std::string s);
/// prelude shared_vector<std::string> str_to_chars(std::string s);
/// prelude i64 str_contains(std::string s, std::string subs);
/// prelude i64 str_len(std::string s);
/// prelude std::string str_remove(std::string s, std::string r);
/// prelude std::string str_arr_join(shared_vector<std::string> a, std::string delimiter);
/// prelude std::string int_to_str(i64 i);
/// prelude void err(std::string s);
/// prelude std::string readln();
/// prelude shared_vector<std::string> read_string_lines_from_file(std::string filename);
void run() {
    shared_vector<i64> a;
    a = create_shared_vector<i64>();
    push(a, 1ll);
    printf("%s %lld\n", ("done"s).c_str(), (i64)(len(a)));
}


