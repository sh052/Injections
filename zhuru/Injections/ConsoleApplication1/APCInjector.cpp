#include "APCInjector.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

namespace InjectionFramework {

    bool APCInjector::inject(const std::string& process_name, const std::string& dll_path) {
        HANDLE process_handle = get_process_handle(process_name);
        if (!process_handle) {
            std::cerr << "�޷��ҵ�ָ�����̡�" << std::endl;
            return false;
        }

      
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            std::cerr << "�޷������߳̿���: " << GetLastError() << std::endl;
            return false;
        }

        THREADENTRY32 thread_entry;
        thread_entry.dwSize = sizeof(THREADENTRY32);
        DWORD target_thread_id = 0;

        if (Thread32First(snapshot, &thread_entry)) {
            do {
                if (thread_entry.th32OwnerProcessID == GetProcessId(process_handle)) {
                    target_thread_id = thread_entry.th32ThreadID;
                    break;
                }
            } while (Thread32Next(snapshot, &thread_entry));
        }

        CloseHandle(snapshot);

        if (target_thread_id == 0) {
            std::cerr << "�Ҳ���Ŀ����̵��̡߳�" << std::endl;
            return false;
        }

        HANDLE thread_handle = OpenThread(THREAD_SET_CONTEXT, FALSE, target_thread_id);
        if (!thread_handle) {
            std::cerr << "�޷����߳�: " << GetLastError() << std::endl;
            return false;
        }

        LPVOID remote_memory = VirtualAllocEx(process_handle, NULL, dll_path.size() + 1, MEM_COMMIT, PAGE_READWRITE);
        if (!remote_memory) {
            std::cerr << "�޷���Ŀ������з����ڴ�: " << GetLastError() << std::endl;
            return false;
        }

        if (!WriteProcessMemory(process_handle, remote_memory, dll_path.c_str(), dll_path.size() + 1, NULL)) {
            std::cerr << "�޷���DLL·��д��Ŀ�����: " << GetLastError() << std::endl;
            return false;
        }

        FARPROC load_library_addr = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        if (!load_library_addr) {
            std::cerr << "�޷���ȡLoadLibraryA��ַ��" << std::endl;
            return false;
        }

        if (QueueUserAPC((PAPCFUNC)load_library_addr, thread_handle, (ULONG_PTR)remote_memory) == 0) {
            std::cerr << "APC ����ʧ��: " << GetLastError() << std::endl;
            return false;
        }

        CloseHandle(thread_handle);
        CloseHandle(process_handle);

        std::cout << "APC ע��ɹ���" << std::endl;
        return true;
    }
}
