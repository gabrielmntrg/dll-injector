#include <Windows.h>

void ShowMessageBox(const char* title, const char* message) {
    MessageBoxA(nullptr, message, title, MB_OK);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        ShowMessageBox("DLL Injected", "DLL Injected");
    }
    return TRUE;
}