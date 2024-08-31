// DllInjectorBase.cpp

#include "DllInjectorBase.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <locale>
#include <codecvt>

namespace InjectionFramework {

    void* DllInjectorBase::get_process_handle(const std::string& process_name) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return nullptr;
        }

        PROCESSENTRY32 process_entry;
        process_entry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshot, &process_entry)) {
            do {
                std::wstring wstrExeFile = process_entry.szExeFile;

                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::string exeFileName = converter.to_bytes(wstrExeFile);

                if (exeFileName == process_name) {
                    HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
                    CloseHandle(snapshot);
                    return process_handle;
                }
            } while (Process32Next(snapshot, &process_entry));
        }

        CloseHandle(snapshot);
        return nullptr;
    }

}
