#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

// Function to get the process ID of the target process
DWORD getProcessId(const char* processName) {
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32First(snapshot, &processEntry)) {
        do {
            if (strcmp(processEntry.szExeFile, processName) == 0) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return 0;
}

// Function to inject the DLL into the target process
bool injectDLL(DWORD processId, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess == NULL) {
        return false;
    }

    void* allocatedMemory = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (allocatedMemory == NULL) {
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, allocatedMemory, dllPath, strlen(dllPath) + 1, NULL)) {
        VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocatedMemory, 0, NULL);
    if (hThread == NULL) {
        VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}

// Main function
int main() {
    const char* processName = "Target.exe";  // Rename to the name of your target process
    const char* dllPath = "C:\\path\\to\\InjectedDLL.dll";  // Rename to the path of your DLL

    DWORD processId = getProcessId(processName);
    if (processId == 0) {
        std::cerr << "Failed to find the target process." << std::endl;
        return 1;
    }

    if (!injectDLL(processId, dllPath)) {
        std::cerr << "DLL injection failed." << std::endl;
        return 1;
    }

    std::cout << "DLL injected successfully." << std::endl;
    return 0;
}