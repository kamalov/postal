#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

#include "my_lib.h"

struct B {
    double value;
};

struct A {
    std::vector<B*>* b;
};

void run() {
    std::vector<long long>* a = new std::vector<long long>();
    long long b;
    long long d;
    A* aa = new A();
    a->push_back(1);
    b = a->at(0);
    printf("%lld\n", static_cast<long long>(b));
    // a = T {}
    // push(a.b, 1)
    // log(a.b[1])
    d = 1;
    d = d + d*d;
    aa = new A();
    aa->b->push_back(1);
    aa->b->at(0)->value = 1;
    printf("%f\n", aa->b->at(0)->value);
}



int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 10);

    try {
        run();
    } catch (const std::string& ex) {
        SetConsoleTextAttribute(hConsole, 12);
        printf("%s\n", ex.c_str());
        return -1;
    } catch (...) {
        SetConsoleTextAttribute(hConsole, 12);
        printf("exception\n");
        return -1;
    }
    
    SetConsoleTextAttribute(hConsole, 7);
    return 0;
}
