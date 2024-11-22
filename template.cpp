#include <stdio.h>
#include <vector>
#include <windows.h>
#include <string>

using namespace std::literals;

%GENERATED_CODE%

int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 10);
    run();
    // SetConsoleTextAttribute(hConsole, 7);
    return 0;
}
