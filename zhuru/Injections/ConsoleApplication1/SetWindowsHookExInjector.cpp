#include "SetWindowsHookExInjector.h"
#include <Windows.h>
#include <iostream>

namespace InjectionFramework {

    bool SetWindowsHookExInjector::inject(const std::string& process_name, const std::string& dll_path) {
        HMODULE dll_module = LoadLibraryA(dll_path.c_str());
        if (!dll_module) {
            std::cerr << "加载 DLL 失败: " << GetLastError() << std::endl;
            return false;
        }

        HOOKPROC hook_proc = (HOOKPROC)GetProcAddress(dll_module, "HookCallback");
        if (!hook_proc) {
            std::cerr << "获取钩子函数地址失败: " << GetLastError() << std::endl;
            FreeLibrary(dll_module);
            return false;
        }

        HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, hook_proc, dll_module, 0);
        if (!hook) {
            std::cerr << "设置钩子失败: " << GetLastError() << std::endl;
            FreeLibrary(dll_module);
            return false;
        }

        std::cout << "DLL 使用 SetWindowsHookEx 注入成功！" << std::endl;
        return true;
    }

} // namespace InjectionFramework
