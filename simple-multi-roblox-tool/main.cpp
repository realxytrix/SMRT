#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

HANDLE g_mutex = nullptr;
std::vector<DWORD> g_processes;

std::string GetTimestamp() { // get the typestamp :p
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "[%H:%M:%S]");
    return ss.str();
}

std::vector<DWORD> GetRobloxProcesses() {
    std::vector<DWORD> processes;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(snapshot, &pe)) {
            do {
                if (wcscmp(pe.szExeFile, L"RobloxPlayerBeta.exe") == 0) {
                    processes.push_back(pe.th32ProcessID);
                }
            } while (Process32Next(snapshot, &pe));
        }
        CloseHandle(snapshot);
    }
    return processes;
}

void UpdateProcesses() {
    std::vector<DWORD> current = GetRobloxProcesses();
    for (DWORD pid : current) {
        if (std::find(g_processes.begin(), g_processes.end(), pid) == g_processes.end()) {
            std::cout << GetTimestamp() << " Roblox started: " << pid << std::endl;
        }
    }
    for (DWORD old : g_processes) {
        if (std::find(current.begin(), current.end(), old) == current.end()) {
            std::cout << GetTimestamp() << " Roblox closed: " << old << std::endl;
        }
    }
    g_processes = current;
}

int main() {
    SetConsoleTitleA("Simple Multi Roblox Tool (SMRT)");
    std::cout << "========================================" << std::endl;
    std::cout << "    Simple Multi Roblox Tool (SMRT)    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << GetTimestamp() << " Starting..." << std::endl;
    g_mutex = CreateMutexA(nullptr, TRUE, "ROBLOX_singletonMutex");
    if (!g_mutex) {
        std::cout << GetTimestamp() << " Failed to create mutex. Error: " << GetLastError() << std::endl;
		std::cout << GetTimestamp() << " Press any key to exit..." << std::endl; // exiting the program if the mutex creation fails
        std::cin.get();
        return 1;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxA(nullptr, "Simple Multi Roblox Tool (SMRT) is already running!", "SMRT", MB_OK | MB_ICONWARNING);
        CloseHandle(g_mutex);
        return 1;
    }
    std::cout << GetTimestamp() << " Ready!" << std::endl;
    g_processes = GetRobloxProcesses(); // initial function
    try {
        while (true) {
            UpdateProcesses();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::exception& ex) {
        std::cout << GetTimestamp() << " An error occurred in monitoring loop: " << ex.what() << std::endl;
    }
    catch (...) {
        std::cout << GetTimestamp() << " An unknown error occurred in monitoring loop." << std::endl; // if the error is unknown it will say it here.
    }

    if (g_mutex) { // cleanup
        ReleaseMutex(g_mutex);
        CloseHandle(g_mutex);
    }

    return 0;
}