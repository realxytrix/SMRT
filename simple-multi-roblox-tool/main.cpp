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

std::string timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "[%H:%M:%S]");
    return ss.str();
}

std::vector<DWORD> getroblox() {
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

void cleanupmutex() {
    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "ROBLOX_singletonMutex");
    if (hMutex) {
        std::cout << timestamp() << " Clean up any stuck mutex.. ;)" << std::endl;
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    SetConsoleTitleA("Simple Multi Roblox Tool (SMRT)");
    std::cout << "========================================" << std::endl;
    std::cout << "    Simple Multi Roblox Tool (SMRT)    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << timestamp() << " Starting..." << std::endl;
    cleanupmutex();
    g_mutex = CreateMutexA(nullptr, FALSE, "ROBLOX_singletonMutex");
    if (!g_mutex) {
        std::cout << timestamp() << " Failed to create mutex. Error: " << GetLastError() << std::endl;
        std::cout << timestamp() << " Press any key to exit..." << std::endl;
        std::cin.get();
        return 1;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cout << timestamp() << " SMRT is already running!" << std::endl;
        cleanupmutex();
        g_mutex = CreateMutexA(nullptr, FALSE, "ROBLOX_singletonMutex");
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            MessageBoxA(nullptr, "SMRT is already open..", "SMRT", MB_OK | MB_ICONWARNING);
            CloseHandle(g_mutex);
            return 1;
        }
    }
    std::cout << timestamp() << " Ready!" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << timestamp() << " Be sure that no roblox instances are running before this tool is running.." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << timestamp() << " IF ROBLOX INSTANCES ARE ALREADY OPEN PLEASE CLOSE THEM AND REOPEN THE TOOL!" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << timestamp() << " DO NOT CLOSE THIS WINDOW!! OR THE BYPASS WILL NOT WORK!! OR ALL OF ROBLOX DUPED INSTANCES WILL BE CLOSED!!" << std::endl;
    g_processes = getroblox();
    try {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    catch (const std::exception& ex) {
        std::cout << timestamp() << " An error occurred: " << ex.what() << std::endl;
    }
    catch (...) {
        std::cout << timestamp() << " An unknown error occurred." << std::endl;
    }
    // cleanup lol
    if (g_mutex) {
        ReleaseMutex(g_mutex);
        CloseHandle(g_mutex);
    }
    return 0;
}