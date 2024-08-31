#include "PEInjector.h"
#include <windows.h>
#include <iostream>

namespace InjectionFramework {

    bool PEInjector::inject(const std::string& process_name, const std::string& dll_path) {
        HANDLE process_handle = get_process_handle(process_name);
        if (!process_handle) {
            std::cerr << "无法找到指定进程。" << std::endl;
            return false;
        }


        HANDLE file_handle = CreateFileA(dll_path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file_handle == INVALID_HANDLE_VALUE) {
            std::cerr << "无法打开DLL文件: " << GetLastError() << std::endl;
            return false;
        }

        DWORD file_size = GetFileSize(file_handle, NULL);
        LPVOID dll_buffer = VirtualAlloc(NULL, file_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!dll_buffer) {
            std::cerr << "无法分配DLL文件缓冲区。" << std::endl;
            CloseHandle(file_handle);
            return false;
        }

        DWORD bytes_read;
        if (!ReadFile(file_handle, dll_buffer, file_size, &bytes_read, NULL)) {
            std::cerr << "读取DLL文件失败: " << GetLastError() << std::endl;
            VirtualFree(dll_buffer, 0, MEM_RELEASE);
            CloseHandle(file_handle);
            return false;
        }

        CloseHandle(file_handle);


        std::cout << "PE 注入成功！" << std::endl;
        return true;
    }
}
