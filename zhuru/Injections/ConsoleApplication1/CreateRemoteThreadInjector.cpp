#include "CreateRemoteThreadInjector.h"
#include <algorithm> // ���� transform �� remove_if
#include <cctype>    // ���� isspace
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <windows.h>

std::string remove_spaces(const std::string& str) {
    std::string no_spaces = str;
    no_spaces.erase(std::remove_if(no_spaces.begin(), no_spaces.end(), ::isspace), no_spaces.end());
    return no_spaces;
}

std::string to_lower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    return lower_str;
}

namespace InjectionFramework {

   
    std::string wstring_to_string(const std::wstring& wstr) {
        if (wstr.empty()) return "";

        int buffer_size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (buffer_size <= 0) {
            std::cerr << "ת�����ַ������ֽ�ʧ�ܡ�" << std::endl;
            return "";
        }

        std::string str(buffer_size - 1, '\0'); // ������С�����������Ŀ��ַ�
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], buffer_size, nullptr, nullptr);
        return str;
    }
    
    HANDLE CreateRemoteThreadInjector::get_process_handle(const std::string& process_name) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            std::cerr << "�޷��������̿��ա�" << std::endl;
            return nullptr;
        }
        std::string lower_process_name = to_lower(remove_spaces(process_name));

        PROCESSENTRY32 process_entry = { sizeof(PROCESSENTRY32) };
        if (Process32First(snapshot, &process_entry)) {
            do {
                std::wstring wstrExeFile(process_entry.szExeFile);  // ��ȡ���ַ��ļ���
                std::string exe_file_name = wstring_to_string(wstrExeFile);
                std::string lower_exe_file_name = to_lower(remove_spaces(exe_file_name));
                std::cout << "������: '" << exe_file_name << "' (����: " << exe_file_name.length() << ")" << std::endl;

                if (lower_exe_file_name.find(lower_process_name) != std::string::npos) {
                    HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
                    if (!process_handle) {
                        std::cerr << "�򿪽���ʧ��: " << GetLastError() << std::endl;
                    }
                    CloseHandle(snapshot);
                    return process_handle;
                }
            } while (Process32Next(snapshot, &process_entry));
        }

        std::cerr << "û���ҵ�ƥ��Ľ���: " << process_name << std::endl;
        CloseHandle(snapshot);
        return nullptr;
    }


    bool CreateRemoteThreadInjector::inject(const std::string& process_name, const std::string& dll_path) {
        HANDLE process_handle = get_process_handle(process_name);
        if (!process_handle) {
            std::cerr << "�޷��ҵ�����: " << process_name << std::endl;
            return false;
        }

        void* remote_memory = VirtualAllocEx(process_handle, nullptr, dll_path.size() + 1, MEM_COMMIT, PAGE_READWRITE);
        if (!remote_memory) {
            std::cerr << "Զ���ڴ����ʧ�ܡ�" << std::endl;
            CloseHandle(process_handle);
            return false;
        }

        if (!WriteProcessMemory(process_handle, remote_memory, dll_path.c_str(), dll_path.size() + 1, nullptr)) {
            std::cerr << "д��Զ�̽����ڴ�ʧ��: " << GetLastError() << std::endl;
            VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        if (!kernel32) {
            std::cerr << "�޷���ȡ kernel32.dll ���: " << GetLastError() << std::endl;
            VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        FARPROC load_library_addr = GetProcAddress(kernel32, "LoadLibraryA");
        if (!load_library_addr) {
            std::cerr << "�޷���ȡ LoadLibraryA ��ַ: " << GetLastError() << std::endl;
            VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        HANDLE thread_handle = CreateRemoteThread(process_handle, nullptr, 0, (LPTHREAD_START_ROUTINE)load_library_addr, remote_memory, 0, nullptr);
        if (!thread_handle) {
            std::cerr << "����Զ���߳�ʧ��: " << GetLastError() << std::endl;
            VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        WaitForSingleObject(thread_handle, INFINITE);
        CloseHandle(thread_handle);
        VirtualFreeEx(process_handle, remote_memory, 0, MEM_RELEASE);
        CloseHandle(process_handle);

        std::cout << "DLL ע��ɹ���" << std::endl;
        return true;
    }

}
