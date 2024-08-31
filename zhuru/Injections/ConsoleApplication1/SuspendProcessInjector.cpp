#include "SuspendProcessInjector.h"
#include <TlHelp32.h>
#include <iostream>
#include <tchar.h>
namespace InjectionFramework {

    bool SuspendProcessInjector::inject(const std::string& process_name, const std::string& dll_path) {
      
        HANDLE process_handle = get_process_handle(process_name);
        if (!process_handle) {
            std::cerr << "无法找到进程: " << process_name << std::endl;
            return false;
        }

        DWORD process_id = GetProcessId(process_handle);
        if (!suspend_threads(process_id)) {
            std::cerr << "无法挂起进程中的所有线程。" << std::endl;
            CloseHandle(process_handle);
            return false;
        }

        LPVOID remote_memory = VirtualAllocEx(process_handle, NULL, dll_path.size() + 1, MEM_COMMIT, PAGE_READWRITE);
        if (!remote_memory) {
            std::cerr << "无法在目标进程中分配内存: " << GetLastError() << std::endl;
            resume_threads(process_id);
            CloseHandle(process_handle);
            return false;
        }

        if (!WriteProcessMemory(process_handle, remote_memory, dll_path.c_str(), dll_path.size() + 1, NULL)) {
            std::cerr << "无法将DLL路径写入目标进程: " << GetLastError() << std::endl;
            resume_threads(process_id);
            VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0,
            (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
            remote_memory, 0, NULL);

        if (!thread_handle) {
            std::cerr << "无法创建远程线程: " << GetLastError() << std::endl;
            resume_threads(process_id);
            VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        WaitForSingleObject(thread_handle, INFINITE);
        CloseHandle(thread_handle);

        if (!resume_threads(process_id)) {
            std::cerr << "无法恢复进程中的所有线程。" << std::endl;
            VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
        CloseHandle(process_handle);
        return true;
    }

    bool SuspendProcessInjector::suspend_threads(DWORD process_id) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return false;
        }

        THREADENTRY32 thread_entry;
        thread_entry.dwSize = sizeof(THREADENTRY32);

        if (Thread32First(snapshot, &thread_entry)) {
            do {
                if (thread_entry.th32OwnerProcessID == process_id) {
                    HANDLE thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread_entry.th32ThreadID);
                    if (thread_handle) {
                        SuspendThread(thread_handle);
                        CloseHandle(thread_handle);
                    }
                }
            } while (Thread32Next(snapshot, &thread_entry));
        }
        CloseHandle(snapshot);
        return true;
    }

    bool SuspendProcessInjector::resume_threads(DWORD process_id) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return false;
        }

        THREADENTRY32 thread_entry;
        thread_entry.dwSize = sizeof(THREADENTRY32);

        if (Thread32First(snapshot, &thread_entry)) {
            do {
                if (thread_entry.th32OwnerProcessID == process_id) {
                    HANDLE thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread_entry.th32ThreadID);
                    if (thread_handle) {
                        ResumeThread(thread_handle);
                        CloseHandle(thread_handle);
                    }
                }
            } while (Thread32Next(snapshot, &thread_entry));
        }
        CloseHandle(snapshot);
        return true;
    }
  
    HANDLE SuspendProcessInjector::get_process_handle(const std::string& process_name) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return nullptr;
        }

        PROCESSENTRY32 process_entry;
        process_entry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshot, &process_entry)) {
            do {
           
                std::wstring exe_name(process_entry.szExeFile);

                std::wstring w_process_name(process_name.begin(), process_name.end());

                if (exe_name == w_process_name) {
                    HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
                    CloseHandle(snapshot);
                    return process_handle;
                }
            } while (Process32Next(snapshot, &process_entry));
        }
        CloseHandle(snapshot);
        return nullptr;
    }

} // namespace InjectionFramework
