#include <iostream>
#include <windows.h>
using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    cout << "Process ID: " << GetCurrentProcessId() << endl;
    
    while (true) {
        Sleep(1000);
    }

    return 0;
}