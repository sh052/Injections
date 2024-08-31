#include "PEInjector.h"
#include <windows.h>
#include <iostream>

namespace InjectionFramework {

    bool PEInjector::inject(const std::string& process_name, const std::string& dll_path) {
        HANDLE process_handle = get_process_handle(process_name);
        if (!process_handle) {
            std::cerr << "�޷��ҵ�ָ�����̡�" << std::endl;
            return false;
        }


        HANDLE file_handle = CreateFileA(dll_path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file_handle == INVALID_HANDLE_VALUE) {
            std::cerr << "�޷���DLL�ļ�: " << GetLastError() << std::endl;
            return false;
        }

        DWORD file_size = GetFileSize(file_handle, NULL);
        LPVOID dll_buffer = VirtualAlloc(NULL, file_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!dll_buffer) {
            std::cerr << "�޷�����DLL�ļ���������" << std::endl;
            CloseHandle(file_handle);
            return false;
        }

        DWORD bytes_read;
        if (!ReadFile(file_handle, dll_buffer, file_size, &bytes_read, NULL)) {
            std::cerr << "��ȡDLL�ļ�ʧ��: " << GetLastError() << std::endl;
            VirtualFree(dll_buffer, 0, MEM_RELEASE);
            CloseHandle(file_handle);
            return false;
        }

        CloseHandle(file_handle);


        std::cout << "PE ע��ɹ���" << std::endl;
        return true;
    }
}
