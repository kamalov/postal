#include "prelude.h"

struct StrField {
    i64 row_count;
    i64 col_count;
    std::vector<std::string>* values;
    friend bool operator==(const StrField& l, const StrField& r) {
        return (l.row_count == r.row_count) && (l.col_count == r.col_count);
    }
};

namespace std {
    template<>
    struct hash<StrField> {
        std::size_t operator()(const StrField& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.row_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.col_count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

struct Cell {
    i64 ri;
    i64 ci;
    friend bool operator==(const Cell& l, const Cell& r) {
        return (l.ri == r.ri) && (l.ci == r.ci);
    }
};

namespace std {
    template<>
    struct hash<Cell> {
        std::size_t operator()(const Cell& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.ri) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ci) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

StrField* create_str_field(i64 row_count, i64 col_count) {
    StrField* f;
    f = new StrField();
    f->row_count = row_count;
    f->col_count = col_count;
    f->values = new std::vector<std::string>();
    array_set_len(f->values, row_count*col_count);
    return f;
}

std::string get_str_field_value(StrField* f, i64 row_index, i64 col_index) {
    return (*f->values)[row_index*f->col_count + col_index];
}

void set_str_field_value(StrField* f, i64 row_index, i64 col_index, std::string value) {
    (*f->values)[row_index*f->col_count + col_index] = value;
}

i64 is_valid_field_index(StrField* f, i64 row_index, i64 col_index) {
    return row_index >= 0ll && row_index < f->row_count && col_index >= 0ll && col_index < f->col_count;
}

void print_str_field(StrField* f) {
    i64 ri;
    std::string s;
    i64 ci;
    std::string v;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;
        s = ""s;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            v = get_str_field_value(f, ri, ci);
            s = s + " "s + v;
        }
        printf("%s\n", (s).c_str());
    }
    printf("\n");
}

StrField* lines_to_str_field(std::vector<std::string>* lines) {
    StrField* f;
    i64 ri;
    i64 ci;
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

void set_visited(std::vector<i64>* visited, i64 ri, i64 ci) {
    (*visited)[ri*1000ll + ci] = 1ll;
}

i64 was_visited(std::vector<i64>* visited, i64 ri, i64 ci) {
    return (*visited)[ri*1000ll + ci];
}

void fill_farm(StrField* f, std::vector<i64>* visited, std::string letter, std::vector<Cell*>* farm, i64 ri, i64 ci) {
    std::string v;
    Cell* cell;

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

std::vector<Cell*>* get_farm(StrField* f, std::vector<i64>* visited, i64 ri, i64 ci) {
    std::vector<Cell*>* farm;
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
    i64 ri;
    i64 ci;
    i64 ri1;
    i64 ci1;
    friend bool operator==(const Edge& l, const Edge& r) {
        return (l.ri == r.ri) && (l.ci == r.ci) && (l.ri1 == r.ri1) && (l.ci1 == r.ci1);
    }
};

namespace std {
    template<>
    struct hash<Edge> {
        std::size_t operator()(const Edge& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.ri) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ci) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ri1) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ci1) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

Edge* coord_to_edge(i64 ri, i64 ci, i64 ri1, i64 ci1) {
    Edge* e;
    e = new Edge();
    e->ri = ri;
    e->ci = ci;
    e->ri1 = ri1;
    e->ci1 = ci1;
    return e;
}

std::vector<Edge*>* get_cell_edges(Cell* f) {
    std::vector<Edge*>* edges;
    edges = new std::vector<Edge*>();
    push(edges, coord_to_edge(f->ri, f->ci, f->ri, f->ci + 1ll));
    push(edges, coord_to_edge(f->ri, f->ci, f->ri + 1ll, f->ci));
    push(edges, coord_to_edge(f->ri + 1ll, f->ci, f->ri + 1ll, f->ci + 1ll));
    push(edges, coord_to_edge(f->ri, f->ci + 1ll, f->ri + 1ll, f->ci + 1ll));
    return edges;
}

std::vector<Edge*>* get_edges(std::vector<Cell*>* cells) {
    universal_hashmap<Edge*, i64>* edges;
    edges = new universal_hashmap<Edge*, i64>();

    auto __expr0 = cells;
    for (int cells__it0__idx = 0; cells__it0__idx < __expr0->size(); cells__it0__idx++) {
        Cell* cells__it0 = (*__expr0)[cells__it0__idx];

        auto __expr1 = get_cell_edges(cells__it0);
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            Edge* expr__it1 = (*__expr1)[expr__it1__idx];

            if (hashmap_has_key(edges, expr__it1)) {
                hashmap_remove(edges, expr__it1);
            }
            else {
                hashmap_add(edges, expr__it1, 0ll);
            };
        }
    }
    return hashmap_keys(edges);
}

struct Point {
    i64 ri;
    i64 ci;
    friend bool operator==(const Point& l, const Point& r) {
        return (l.ri == r.ri) && (l.ci == r.ci);
    }
};

namespace std {
    template<>
    struct hash<Point> {
        std::size_t operator()(const Point& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.ri) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<i64>{}(rec.ci) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

struct EdgeArray {
    i64 dummy;
    std::vector<Edge*>* edges;
    friend bool operator==(const EdgeArray& l, const EdgeArray& r) {
        return (l.dummy == r.dummy);
    }
};

namespace std {
    template<>
    struct hash<EdgeArray> {
        std::size_t operator()(const EdgeArray& rec) const {
            size_t h = 0;
            h ^= std::hash<i64>{}(rec.dummy) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

void update_point(universal_hashmap<Point*, EdgeArray*>* points, Point* p, Edge* edge) {
    EdgeArray* arr;

    if (hashmap_has_key(points, p)) {
        arr = hashmap_get_value(points, p);
        push(arr->edges, edge);
    }
    else {
        arr = new EdgeArray();
        arr->edges = new std::vector<Edge*>();
        push(arr->edges, edge);
        hashmap_add(points, p, arr);
    };
}

i64 is_edge_horizontal(Edge* edge) {
    return edge->ri == edge->ri1;
}

i64 get_sides(std::vector<Cell*>* cells) {
    std::vector<Edge*>* edges;
    universal_hashmap<Point*, EdgeArray*>* points_hashmap;
    Edge* edge;
    Point* p1;
    Point* p2;
    i64 angles;
    EdgeArray* arr;
    edges = get_edges(cells);
    points_hashmap = new universal_hashmap<Point*, EdgeArray*>();

    auto __expr0 = edges;
    for (int edges__it0__idx = 0; edges__it0__idx < __expr0->size(); edges__it0__idx++) {
        Edge* edges__it0 = (*__expr0)[edges__it0__idx];
        edge = edges__it0;
        p1 = new Point();
        p1->ri = edges__it0->ri;
        p1->ci = edges__it0->ci;
        p2 = new Point();
        p2->ri = edges__it0->ri1;
        p2->ci = edges__it0->ci1;
        update_point(points_hashmap, p1, edge);
        update_point(points_hashmap, p2, edge);
    }
    angles = 0ll;

    auto __expr1 = hashmap_keys(points_hashmap);
    for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
        Point* expr__it1 = (*__expr1)[expr__it1__idx];
        arr = hashmap_get_value(points_hashmap, expr__it1);

        if (len(arr->edges) < 2ll) {
            err("wrong number of edges"s);
        };

        if (len(arr->edges) == 4ll) {
            angles = angles + 2ll;
        }
        else {

            if (is_edge_horizontal((*arr->edges)[0ll]) != is_edge_horizontal((*arr->edges)[1ll])) {
                angles = angles + 1ll;
            };
        };
        //log(len(arr.edges))
    }
    return angles;
}

i64 process(StrField* f) {
    std::vector<i64>* visited;
    i64 total;
    i64 ri;
    i64 ci;
    std::vector<Cell*>* farm;
    std::string v;
    i64 sides;
    visited = new std::vector<i64>();
    array_set_len(visited, 1000000ll);
    total = 0ll;

    auto __expr0 = create_range(0ll, (f->row_count - 1ll));
    for (int expr__it0__idx = 0; expr__it0__idx < __expr0->size(); expr__it0__idx++) {
        i64 expr__it0 = (*__expr0)[expr__it0__idx];
        ri = expr__it0;

        auto __expr1 = create_range(0ll, (f->col_count - 1ll));
        for (int expr__it1__idx = 0; expr__it1__idx < __expr1->size(); expr__it1__idx++) {
            i64 expr__it1 = (*__expr1)[expr__it1__idx];
            ci = expr__it1;
            farm = get_farm(f, visited, ri, ci);

            if (len(farm) > 0ll) {
                v = get_str_field_value(f, ri, ci);
                sides = get_sides(farm);
                printf("%s %s %lld %s %lld\n", (v).c_str(), ("len: "s).c_str(), static_cast<i64>(len(farm)), ("; sides: "s).c_str(), static_cast<i64>(sides));
                total = total + sides*len(farm);
            };
        }
    }
    return total;
}

void run() {
    std::vector<std::string>* lines;
    StrField* f;
    i64 total;
    lines = read_string_lines_from_file("D:/src/postal/aoc2024/input.txt"s);
    f = lines_to_str_field(lines);
    print_str_field(f);
    total = process(f);
    printf("%s %lld\n", ("total = "s).c_str(), static_cast<i64>(total));
}


