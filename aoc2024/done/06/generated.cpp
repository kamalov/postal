#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

/// lib array utils
template <typename T>long long len(std::vector<T>* a);
template <typename T>void set_array_size(std::vector<T>* a, long long new_size);
template <typename T>void push(std::vector<T>* a, T elem);
template <typename T>void set_array_value(std::vector<T>* a, long long index, T value);
template <typename T>void sort(std::vector<T>* a);
template <typename T>long long array_contains(std::vector<T>* a, T value);
/// lib string utils
std::vector<std::string>* split_str(std::string s, std::string by);
long long str_to_int(std::string s);
std::string int_to_str(long long i);
std::vector<std::string>* str_to_chars(std::string s);
long long str_contains(std::string s, std::string subs);
/// lib other utils
void err(std::string s);
std::string read_line();
std::vector<std::string>* read_string_lines_from_file(std::string filename);
/// main
struct Row {
    std::vector<std::string>* cols;
};

struct Field {
    std::vector<Row*>* rows;
    long long max_rows_index;
    long long max_cols_index;
    std::string current_dir;
    long long ri;
    long long ci;
};

Field* clone_field(Field* f) {
    Field* nf = new Field();
    Row* it_row = new Row();
    Row* row = new Row();
    nf = new Field();
    nf->max_rows_index = f->max_rows_index;
    nf->max_cols_index = f->max_cols_index;
    nf->current_dir = f->current_dir;
    nf->ri = f->ri;
    nf->ci = f->ci;
    nf->rows = new std::vector<Row*>();

    auto _expr0 = f->rows;
    for (int exp__it0__idx = 0; exp__it0__idx < _expr0->size(); exp__it0__idx++) {
        Row* exp__it0 = (*_expr0)[exp__it0__idx];
        it_row = exp__it0;
        row = new Row();
        row->cols = new std::vector<std::string>();

        auto _expr1 = it_row->cols;
        for (int exp__it1__idx = 0; exp__it1__idx < _expr1->size(); exp__it1__idx++) {
            std::string exp__it1 = (*_expr1)[exp__it1__idx];
            push(row->cols, exp__it1);
        }
        push(nf->rows, row);
    }
    return nf;
}

Field* init_field(std::vector<std::string>* lines) {
    Field* field = new Field();
    long long row_index;
    Row* row = new Row();
    field = new Field();
    field->rows = new std::vector<Row*>();

    auto _expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < _expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*_expr0)[lines__it0__idx];
        row_index = lines__it0__idx;
        row = new Row();
        row->cols = new std::vector<std::string>();
        row->cols = str_to_chars(lines__it0);

        auto _expr1 = row->cols;
        for (int exp__it1__idx = 0; exp__it1__idx < _expr1->size(); exp__it1__idx++) {
            std::string exp__it1 = (*_expr1)[exp__it1__idx];

            if (exp__it1 == "<"s || exp__it1 == ">"s || exp__it1 == "^"s || exp__it1 == "v"s) {
                field->ri = row_index;
                field->ci = exp__it1__idx;
                field->current_dir = exp__it1;
            };
        }
        push(field->rows, row);
    }
    field->max_rows_index = len(field->rows) - 1ll;
    field->max_cols_index = len(field->rows->at(0ll)->cols) - 1ll;
    return field;
}

void print_field(Field* field) {
    std::string s;
    std::vector<std::string>* cols;

    auto _expr0 = field->rows;
    for (int exp__it0__idx = 0; exp__it0__idx < _expr0->size(); exp__it0__idx++) {
        Row* exp__it0 = (*_expr0)[exp__it0__idx];
        s = ""s;
        cols = exp__it0->cols;

        auto _expr1 = cols;
        for (int cols__it1__idx = 0; cols__it1__idx < _expr1->size(); cols__it1__idx++) {
            std::string cols__it1 = (*_expr1)[cols__it1__idx];
            s = s + " "s + cols__it1;
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

std::string get_field_value(Field* f, long long ri, long long ci) {

    if (ri < 0ll || ci < 0ll || ri > f->max_rows_index || ci > f->max_cols_index) {
        return ""s;
    };
    return f->rows->at(ri)->cols->at(ci);
}

void add_field_cell_dir(Field* f, long long ri, long long ci, std::string dir) {
    std::string v;
    v = f->rows->at(ri)->cols->at(ci);

    if (str_contains(v, dir) == 0ll) {
        v = v + dir;
        set_array_value(f->rows->at(ri)->cols, ci, v);
    };
}

struct Delta {
    long long dr;
    long long dc;
};

Delta* get_delta(std::string dir) {
    Delta* d = new Delta();
    d = new Delta();
    d->dr = 0ll;
    d->dc = 0ll;

    if (dir == "^"s) {
        d->dr = 0ll - 1ll;
    }
    else if (dir == "v"s) {
        d->dr = 1ll;
    }
    else if (dir == ">"s) {
        d->dc = 1ll;
    }
    else if (dir == "<"s) {
        d->dc = 0ll - 1ll;
    };
    return d;
}

std::string rotate(std::string dir) {

    if (dir == "^"s) {
        return ">"s;
    };

    if (dir == ">"s) {
        return "v"s;
    };

    if (dir == "v"s) {
        return "<"s;
    };

    if (dir == "<"s) {
        return "^"s;
    };
    err("wrong rotate "s + dir);
}

long long can_find_exit_without_check(Field* f) {
    Delta* d = new Delta();
    long long next_ri;
    long long next_ci;
    std::string next_cell_value;

    for (;;) {
        d = get_delta(f->current_dir);
        next_ri = f->ri + d->dr;
        next_ci = f->ci + d->dc;
        next_cell_value = get_field_value(f, next_ri, next_ci);

        if (next_cell_value == ""s) {
            return 1ll;
        };

        if (next_cell_value == "#"s) {
            f->current_dir = rotate(f->current_dir);
        }
        else {

            if (str_contains(next_cell_value, f->current_dir)) {
                return 0ll;
            };
            f->ri = next_ri;
            f->ci = next_ci;
        };
        add_field_cell_dir(f, f->ri, f->ci, f->current_dir);
    }
}

long long try_obstacle_loop(Field* f, long long ri, long long ci) {
    Field* nf = new Field();
    long long is_loop;
    nf = clone_field(f);
    set_array_value(nf->rows->at(ri)->cols, ci, "#"s);
    is_loop = can_find_exit_without_check(nf) == 0ll;
    return is_loop;
}

long long can_find_exit(Field* f) {
    long long possible_obstacles_count;
    Delta* d = new Delta();
    long long next_ri;
    long long next_ci;
    std::string next_cell_value;
    possible_obstacles_count = 0ll;

    for (;;) {
        d = get_delta(f->current_dir);
        next_ri = f->ri + d->dr;
        next_ci = f->ci + d->dc;
        next_cell_value = get_field_value(f, next_ri, next_ci);

        if (next_cell_value == ""s) {
            return possible_obstacles_count;
        };

        if (next_cell_value == "#"s) {
            f->current_dir = rotate(f->current_dir);
            add_field_cell_dir(f, f->ri, f->ci, f->current_dir);
        }
        else {

            if (str_contains(next_cell_value, f->current_dir)) {
                err("loop, shouldn't  be"s);
            };

            if (next_cell_value == "."s && try_obstacle_loop(f, next_ri, next_ci)) {
                printf("%lld %s %lld %lld\n", static_cast<long long>(possible_obstacles_count), ("loop at obstacle"s).c_str(), static_cast<long long>(next_ri), static_cast<long long>(next_ci));
                possible_obstacles_count = possible_obstacles_count + 1ll;
                //print_field(f)
                //read_line()
            };
            f->ri = next_ri;
            f->ci = next_ci;
            add_field_cell_dir(f, f->ri, f->ci, f->current_dir);
        };
    }
}

long long count_visited(Field* f) {
    long long count;
    long long ri;
    std::string v;
    count = 0ll;

    auto _expr0 = create_range(0ll, f->max_rows_index);
    for (int exp__it0__idx = 0; exp__it0__idx < _expr0->size(); exp__it0__idx++) {
        long long exp__it0 = (*_expr0)[exp__it0__idx];
        ri = exp__it0__idx;

        auto _expr1 = create_range(0ll, f->max_cols_index);
        for (int exp__it1__idx = 0; exp__it1__idx < _expr1->size(); exp__it1__idx++) {
            long long exp__it1 = (*_expr1)[exp__it1__idx];
            v = get_field_value(f, ri, exp__it1__idx);

            if (v == "#"s || v == "."s) {
                continue;
            };
            count = count + 1ll;
        }
    }
    return count;
}

void run() {
    std::vector<std::string>* lines;
    Field* field = new Field();
    long long possible_obstacles_count;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    field = init_field(lines);
    //   print_field(field)
    possible_obstacles_count = can_find_exit(field);
    //   print_field(field)
    printf("%lld %s\n", static_cast<long long>(count_visited(field)), ("visited"s).c_str());
    printf("%lld %s\n", static_cast<long long>(possible_obstacles_count), ("obstacles"s).c_str());
}



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
