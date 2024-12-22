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
/// lib array utils
template <typename T>long long len(std::vector<T>* a);
template <typename T>void push(std::vector<T>* a, T elem);
template <typename T>void array_set_len(std::vector<T>* a, long long new_len);
template <typename T>void array_sort(std::vector<T>* a);
template <typename T>long long array_contains(std::vector<T>* a, T value);
template <typename T>void array_remove_at(std::vector<T>* a, long long index);
template <typename T>void array_remove(std::vector<T>* a, T value);
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
/// main
struct StrField {
    long long row_count;
    long long col_count;
    std::vector<std::string>* values;
    friend bool operator==(const StrField& l, const StrField& r) {
        return (l.row_count == r.row_count) && (l.col_count == r.col_count);
    }
};

struct Cell {
    long long ri;
    long long ci;
    friend bool operator==(const Cell& l, const Cell& r) {
        return (l.ri == r.ri) && (l.ci == r.ci);
    }
};

StrField* create_str_field(long long row_count, long long col_count) {
    StrField* f = new StrField();
    f = new StrField();
    f->row_count = row_count;
    f->col_count = col_count;
    f->values = new std::vector<std::string>();
    array_set_len(f->values, row_count*col_count);
    return f;
}

std::string get_str_field_value(StrField* f, long long row_index, long long col_index) {
    return (*f->values)[row_index*f->col_count + col_index];
}

void set_str_field_value(StrField* f, long long row_index, long long col_index, std::string value) {
    (*f->values)[row_index*f->col_count + col_index] = value;
}

long long is_valid_field_index(StrField* f, long long row_index, long long col_index) {
    return row_index >= 0ll && row_index < f->row_count && col_index >= 0ll && col_index < f->col_count;
}

void print_str_field(StrField* f) {
    long long ri;
    std::string s;
    long long ci;
    std::string v;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        long long expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;
        s = ""s;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_str_field_value(f, ri, ci);
            s = s + " "s + v;
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

StrField* lines_to_str_field(std::vector<std::string>* lines) {
    StrField* f = new StrField();
    long long ri;
    long long ci;
    f = create_str_field(len(lines), str_len((*lines)[0ll]));

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        ri = lines__it0__idx;

        auto __expr1 = str_to_chars(lines__it0);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            std::string expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1__idx;
            set_str_field_value(f, ri, ci, expr__it1);
        }
    }
    return f;
}

void set_visited(std::vector<long long>* visited, long long ri, long long ci) {
    (*visited)[ri*1000ll + ci] = 1ll;
}

long long was_visited(std::vector<long long>* visited, long long ri, long long ci) {
    return (*visited)[ri*1000ll + ci];
}

void fill_farm(StrField* f, std::vector<long long>* visited, std::string letter, std::vector<Cell*>* farm, long long ri, long long ci) {
    std::string v;
    Cell* cell = new Cell();

    if (is_valid_field_index(f, ri, ci) == 0ll) {
        return;
    };

    if (was_visited(visited, ri, ci)) {
        return;
    };
    v = get_str_field_value(f, ri, ci);

    if (v != letter) {
        return;
    };
    cell = new Cell();
    cell->ri = ri;
    cell->ci = ci;
    push(farm, cell);
    set_visited(visited, ri, ci);
    fill_farm(f, visited, letter, farm, ri - 1ll, ci);
    fill_farm(f, visited, letter, farm, ri + 1ll, ci);
    fill_farm(f, visited, letter, farm, ri, ci - 1ll);
    fill_farm(f, visited, letter, farm, ri, ci + 1ll);
}

std::vector<Cell*>* get_farm(StrField* f, std::vector<long long>* visited, long long ri, long long ci) {
    std::vector<Cell*>* farm = new std::vector<Cell*>();
    std::string letter;
    farm = new std::vector<Cell*>();

    if (was_visited(visited, ri, ci) == 1ll) {
        return farm;
    };
    letter = get_str_field_value(f, ri, ci);
    fill_farm(f, visited, letter, farm, ri, ci);
    return farm;
}

struct Edge {
    long long ri;
    long long ci;
    long long ri1;
    long long ci1;
    friend bool operator==(const Edge& l, const Edge& r) {
        return (l.ri == r.ri) && (l.ci == r.ci) && (l.ri1 == r.ri1) && (l.ci1 == r.ci1);
    }
};

Edge* coord_to_edge(long long ri, long long ci, long long ri1, long long ci1) {
    Edge* e = new Edge();
    e = new Edge();
    e->ri = ri;
    e->ci = ci;
    e->ri1 = ri1;
    e->ci1 = ci1;
    return e;
}

std::vector<Edge*>* get_cell_edges(Cell* f) {
    std::vector<Edge*>* edges = new std::vector<Edge*>();
    edges = new std::vector<Edge*>();
    push(edges, coord_to_edge(f->ri, f->ci, f->ri, f->ci + 1ll));
    push(edges, coord_to_edge(f->ri, f->ci, f->ri + 1ll, f->ci));
    push(edges, coord_to_edge(f->ri + 1ll, f->ci, f->ri + 1ll, f->ci + 1ll));
    push(edges, coord_to_edge(f->ri, f->ci + 1ll, f->ri + 1ll, f->ci + 1ll));
    return edges;
}

long long get_perimeter(std::vector<Cell*>* cells) {
    std::vector<Edge*>* edges = new std::vector<Edge*>();
    edges = new std::vector<Edge*>();

    auto __expr0 = cells;
    for (int cells__it0__idx = 0; cells__it0__idx < __expr0->size(); cells__it0__idx++) {
        Cell* cells__it0 = (*__expr0)[cells__it0__idx];

        auto __expr1 = get_cell_edges(cells__it0);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            Edge* expr__it1 = (*__expr1)[expr__it1__idx];

            if (array_contains(edges, expr__it1)) {
                array_remove(edges, expr__it1);
            }
            else {
                push(edges, expr__it1);
            };
        }
    }
    return len(edges);
}

long long process(StrField* f) {
    std::vector<long long>* visited;
    long long total;
    long long ri;
    long long ci;
    std::vector<Cell*>* farm = new std::vector<Cell*>();
    std::string v;
    long long perimeter;
    visited = new std::vector<long long>();
    array_set_len(visited, 1000000ll);
    total = 0ll;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        long long expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            farm = get_farm(f, visited, ri, ci);

            if (len(farm) > 0ll) {
                v = get_str_field_value(f, ri, ci);
                perimeter = get_perimeter(farm);
                printf("%s %s %lld %s %lld\n", (v).c_str(), ("len: "s).c_str(), static_cast<long long>(len(farm)), ("; perimeter: "s).c_str(), static_cast<long long>(perimeter));
                total = total + perimeter*len(farm);
            };
        }
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    StrField* f = new StrField();
    long long total;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    f = lines_to_str_field(lines);
    print_str_field(f);
    total = process(f);
    printf("%s %lld\n", ("total = "s).c_str(), static_cast<long long>(total));
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
