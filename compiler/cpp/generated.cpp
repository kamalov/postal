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
////
//// prelude end
////
void test_multiple_params(i64 a, double b) {
    double x;
    x = a + 1.1;
    printf("%lld %f %s %f\n", (i64)(a), b, ("  ->  "s).c_str(), x);
}

void test_if(i64 a) {

    if (a < 0ll) {
        printf("%s\n", ("a < 0"s).c_str());
    }
    else if (a > 10ll) {
        printf("%s\n", ("a > 10"s).c_str());
    }
    else {
        printf("%s\n", ("a >= 0 and <= 10"s).c_str());
        printf("%lld\n", (i64)(a + 10ll));
    };
}

void test_loop() {
    i64 i;
    i = 0ll;

    for (;;) {
        i = i + 1ll;
        printf("%lld\n", (i64)(i));

        if (i == 10ll) {
            break;
        };
    }
}

void test_iterator() {
    shared_vector<i64> numbers;
    numbers = create_shared_vector<i64>();
    array_push(numbers, 123ll);
    array_push(numbers, 456ll);
    array_push(numbers, 789ll);

    auto __expr0 = numbers;
    for (i64 numbers__it0__idx = 0; numbers__it0__idx < (i64)__expr0->size(); numbers__it0__idx++) {
        i64 numbers__it0 = (*__expr0)[numbers__it0__idx];
        printf("%lld\n", (i64)(numbers__it0__idx));
        printf("%s %lld %s %lld\n", ("numbers "s).c_str(), (i64)(numbers__it0__idx), (" "s).c_str(), (i64)(numbers__it0));

        auto __expr1 = numbers;
        for (i64 numbers__it1__idx = 0; numbers__it1__idx < (i64)__expr1->size(); numbers__it1__idx++) {
            i64 numbers__it1 = (*__expr1)[numbers__it1__idx];
            printf("%s %lld %s %lld\n", ("    "s).c_str(), (i64)(numbers__it1__idx), (" "s).c_str(), (i64)(numbers__it1));
        }
        printf("%s %lld %s %lld\n", ("<< "s).c_str(), (i64)(numbers__it0__idx), (" "s).c_str(), (i64)(numbers__it0));
    }
}

i64 test_params1(i64 a, i64 b) {
    return a + b;
}

void test_params() {
    i64 b;
    b = 5ll + 6ll + 7ll*test_params1(5ll, 6ll);
    printf("%lld\n", (i64)(b));
    b = test_params1(test_params1(5ll, 6ll), 6ll);
    printf("%lld\n", (i64)(b));
}

void test_array_access() {
    shared_vector<double> numbers;
    numbers = create_shared_vector<double>();
    array_push(numbers, 123.0);
    array_push(numbers, 456.0);
    array_push(numbers, 789.0);

    auto __expr0 = numbers;
    for (i64 numbers__it0__idx = 0; numbers__it0__idx < (i64)__expr0->size(); numbers__it0__idx++) {
        double numbers__it0 = (*__expr0)[numbers__it0__idx];
        printf("%f\n", numbers->at(numbers__it0__idx));
    }
}

struct Coordinates {
    double x;
    i64 y;
    friend bool operator==(const Coordinates& l, const Coordinates& r) {
        return (l.x == r.x) && (l.y == r.y);
    }
};

namespace std {
    template<>
    struct hash<Coordinates> {
        std::size_t operator()(const Coordinates& rec) const {
            size_t h = 0;
            h ^= std::hash<double>{}(rec.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

void test_records() {
    shared_pointer<Coordinates> r;
    double a;
    r = create_shared_pointer<Coordinates >();
    r->x = 10ll + 10ll;
    r->y = 20ll;
    printf("%f\n", r->x);
    printf("%lld\n", (i64)(r->y));
    a = r->x*r->y;
    printf("%f\n", r->x*r->y);
    printf("%f\n", a);
    printf("%d\n", r->x*r->y == a);
}

void test_strings() {
    std::string s;
    std::string ss;
    std::string sss;
    s = "123"s;
    ss = "abc"s;
    sss = s + " "s + ss;
    printf("%s\n", (sss).c_str());
}

//// function read_lines(filename: string) [string] external
//// function test_read_lines
////     lines = read_string_lines_from_file("d:/test.txt")
////     for lines do log(index, item)
//// end
void run() {
    test_multiple_params(5ll, 6.0);
    test_if(-1ll);
    test_if(5ll);
    test_if(11ll);
    test_loop();
    test_iterator();
    test_array_access();
    test_records();
    test_strings();
    ////test_read_lines()
    printf("%s\n", ("done"s).c_str());
}

