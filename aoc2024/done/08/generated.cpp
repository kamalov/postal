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
std::string read_line_from_console();
std::vector<std::string>* read_string_lines_from_file(std::string filename);
/// main
struct Row {
    std::vector<std::string>* cols;
};

struct Field {
    std::vector<Row*>* rows;
    long long max_rows_index;
    long long max_cols_index;
    long long ri;
    long long ci;
};

Field* clone_field(Field* f) {
    Field* nf = new Field();
    Row* row = new Row();
    nf = new Field();
    nf->max_rows_index = f->max_rows_index;
    nf->max_cols_index = f->max_cols_index;
    nf->ri = f->ri;
    nf->ci = f->ci;
    nf->rows = new std::vector<Row*>();

    auto __expr0 = f->rows;
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Row* expr__it0 = (*__expr0)[expr__it0__idx];
        row = new Row();
        row->cols = new std::vector<std::string>();

        auto __expr1 = expr__it0->cols;
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            std::string expr__it1 = (*__expr1)[expr__it1__idx];
            push(row->cols, expr__it1);
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

    auto __expr0 = lines;
    for (int lines__it0__idx = 0; lines__it0__idx < __expr0->size(); lines__it0__idx++) {
        std::string lines__it0 = (*__expr0)[lines__it0__idx];
        row_index = lines__it0__idx;
        row = new Row();
        row->cols = new std::vector<std::string>();
        row->cols = str_to_chars(lines__it0);
        push(field->rows, row);
    }
    field->max_rows_index = len(field->rows) - 1ll;
    field->max_cols_index = len(field->rows->at(0ll)->cols) - 1ll;
    return field;
}

void print_field(Field* field) {
    std::string s;

    auto __expr0 = field->rows;
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Row* expr__it0 = (*__expr0)[expr__it0__idx];
        s = ""s;

        auto __expr1 = expr__it0->cols;
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            std::string expr__it1 = (*__expr1)[expr__it1__idx];
            s = s + " "s + expr__it1;
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

long long try_set_field_value(Field* f, long long ri, long long ci, std::string value) {

    if (ri < 0ll || ci < 0ll || ri > f->max_rows_index || ci > f->max_cols_index) {
        return 0ll;
    };
    f->rows->at(ri)->cols->at(ci) = value;
    return 1ll;
}

struct FieldCell {
    std::string v;
    long long ri;
    long long ci;
};

std::vector<FieldCell*>* get_cells(Field* f) {
    std::vector<FieldCell*>* cells = new std::vector<FieldCell*>();
    long long ri;
    FieldCell* fc = new FieldCell();
    cells = new std::vector<FieldCell*>();

    auto __expr0 = f->rows;
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        Row* expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0__idx;

        auto __expr1 = expr__it0->cols;
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            std::string expr__it1 = (*__expr1)[expr__it1__idx];
            fc = new FieldCell();
            fc->v = expr__it1;
            fc->ri = ri;
            fc->ci = expr__it1__idx;
            push(cells, fc);
        }
    }
    return cells;
}

void set_anti(Field* af, FieldCell* c1, FieldCell* c2) {
    long long dr;
    long long dc;
    long long ri;
    long long ci;
    dr = c2->ri - c1->ri;
    dc = c2->ci - c1->ci;
    //log('dr:', dr, 'dc:' + dc)
    ri = c1->ri;
    ci = c1->ci;

    for (;;) {
        ri = ri - dr;
        ci = ci - dc;

        if (try_set_field_value(af, ri, ci, "#"s) == 0ll) {
            break;
        };
    }
    ri = c2->ri;
    ci = c2->ci;

    for (;;) {
        ri = ri + dr;
        ci = ci + dc;

        if (try_set_field_value(af, ri, ci, "#"s) == 0ll) {
            break;
        };
    }
}

long long count_anti(Field* f) {
    Field* af = new Field();
    std::vector<FieldCell*>* cells = new std::vector<FieldCell*>();
    FieldCell* c1 = new FieldCell();
    FieldCell* c2 = new FieldCell();
    long long total;
    af = clone_field(f);
    cells = get_cells(f);

    auto __expr0 = create_range(0ll, (len(cells) - 2ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        long long expr__it0 = (*__expr0)[expr__it0__idx];
        c1 = cells->at(expr__it0);
        //s = int_to_str(it) + ' -> '

        auto __expr1 = create_range((expr__it0 + 1ll), (len(cells) - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            long long expr__it1 = (*__expr1)[expr__it1__idx];
            c2 = cells->at(expr__it1);

            if (c1->v != "."s && c1->v == c2->v) {
                set_anti(af, c1, c2);
                //log(c1.ri, c1.ci, c2.ri, c2.ci)
                // print_field(f)
                // print_field(af)
                // read_line_from_console()
            };
            //s = s + int_to_str(it) + ', '
        }
        //log(s)
    }
    total = 0ll;
    print_field(af);

    auto __expr2 = af->rows;
    for (int expr__it2__idx = 0; expr__it2__idx < __expr2->size(); expr__it2__idx++) {
        Row* expr__it2 = (*__expr2)[expr__it2__idx];

        auto __expr3 = expr__it2->cols;
        for (int expr__it3__idx = 0; expr__it3__idx < __expr3->size(); expr__it3__idx++) {
            std::string expr__it3 = (*__expr3)[expr__it3__idx];

            if (expr__it3 != "."s) {
                total = total + 1ll;
            };
        }
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    Field* field = new Field();
    long long count;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    field = init_field(lines);
    print_field(field);
    count = count_anti(field);
    printf("%lld %s\n", static_cast<long long>(count), ("done"s).c_str());
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
