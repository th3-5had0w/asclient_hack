// asclient_hack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <Psapi.h>
#include <string.h>
#include <TlHelp32.h>

unsigned int getProcId(const wchar_t *procName) {
    unsigned int procID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "Error creating snapshot" << std::endl;
        exit(1);
    }
    PROCESSENTRY32 procEntry;
    procEntry.dwSize = sizeof(procEntry);
    if (Process32First(hSnapshot, &procEntry)) {
        do {
            if (wcscmp(procEntry.szExeFile, procName) == 0) {
                procID = procEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &procEntry));
    }
    return procID;
}

HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, getProcId(L"ac_client.exe"));

unsigned int getModuleBaseAddress(unsigned int procID, const wchar_t *moduleName) {
    unsigned int moduleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "Error creating module snapshot" << std::endl;
        exit(1);
    }
    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(moduleEntry);
    if (Module32First(hSnapshot, &moduleEntry)) {
        do {
            if (wcscmp(moduleEntry.szModule, moduleName) == 0) {
                moduleBaseAddress = (unsigned int)moduleEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &moduleEntry));
    }
    return moduleBaseAddress;
}

unsigned int derefPtr(unsigned int baseAddress, std::vector<unsigned int>offsets) {
    unsigned int tmpAddress = baseAddress;
    for (int i = 0; i < offsets.size(); ++i) {
        tmpAddress += offsets[i];
        if (i != offsets.size() - 1) ReadProcessMemory(hProc, (unsigned int*)tmpAddress, &tmpAddress, sizeof(unsigned int), NULL);
    }
    
    //WriteProcessMemory(hProc, (unsigned int*)tmpAddress, &health, sizeof(health), NULL);
    return tmpAddress;
}

void toggleDoublePistol(unsigned int baseAddr) {
    std::vector<unsigned int>toggle_offsets = {0x00109FCC, 0x0, 0x10C};
    std::vector<unsigned int>double_pistol_ammo_offsets = { 0x00109FCC, 0x0, 0x15C };
    unsigned int toggle = 1;
    unsigned int ammo = 9000000;
    WriteProcessMemory(
        hProc,
        (unsigned int*)derefPtr(baseAddr, toggle_offsets),
        &toggle,
        sizeof(toggle),
        NULL);
    WriteProcessMemory(
        hProc,
        (unsigned int*)derefPtr(baseAddr, double_pistol_ammo_offsets),
        &ammo,
        sizeof(ammo),
        NULL);
}

void toggleHealth(unsigned int baseAddr) {
    unsigned int health = 9000000;
    std::vector<unsigned int>health_offsets = { 0x00109FCC, 0x0, 0xF8 };
    WriteProcessMemory(
        hProc,
        (unsigned int*)derefPtr(baseAddr, health_offsets),
        &health,
        sizeof(health),
        NULL
    );
}

void toggleGrenade(unsigned int baseAddr) {
    unsigned int grenade = 10000;
    std::vector<unsigned int>grenade_offsets = { 0x00109FCC, 0x0, 0x158 };
    WriteProcessMemory(hProc,
        (unsigned int*)derefPtr(baseAddr, grenade_offsets),
        &grenade,
        sizeof(grenade),
        NULL
    );
}

void togglePrimaryGunAmmo(unsigned int baseAddr) {
    unsigned int ammo = 9000000;
    std::vector<unsigned int>ammo_offsets = { 0x00109FCC, 0x0, 0x150 };
    WriteProcessMemory(
        hProc,
        (unsigned int*)derefPtr(baseAddr, ammo_offsets),
        &ammo,
        sizeof(ammo),
        NULL);
}

int main()
{
    unsigned int baseAddr = getModuleBaseAddress(getProcId(L"ac_client.exe"), L"ac_client.exe");
    toggleDoublePistol(baseAddr);
    toggleHealth(baseAddr);
    toggleGrenade(baseAddr);
    togglePrimaryGunAmmo(baseAddr);
    std::cout << "Hello World!\n";
}



























