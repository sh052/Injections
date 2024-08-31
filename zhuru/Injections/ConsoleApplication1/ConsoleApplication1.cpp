#include <iostream>
#include <string>
#include "CreateRemoteThreadInjector.h"
#include "SetWindowsHookExInjector.h"
#include "SuspendProcessInjector.h"
#include "APCInjector.h"
#include "PEInjector.h"
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "用法: " << argv[0] << " <进程名> <DLL路径> <注入方式>" << std::endl;
        std::cerr << "注入方式: CreateRemoteThread, SetWindowsHookEx, SuspendProcess" << std::endl;
        return 1;
    }

    std::string process_name = argv[1];
    std::string dll_path = argv[2];
    std::string inject_method = argv[3];

    bool success = false;

    if (inject_method == "CreateRemoteThread") {
        InjectionFramework::CreateRemoteThreadInjector injector;
        success = injector.inject(process_name, dll_path);
    }
    else if (inject_method == "SetWindowsHookEx") {
        InjectionFramework::SetWindowsHookExInjector injector;
        success = injector.inject(process_name, dll_path);
    }
    else if (inject_method == "SuspendProcess") {
        // 使用相同的方式来创建 SuspendProcessInjector 对象
        InjectionFramework::SuspendProcessInjector injector;
        success = injector.inject(process_name, dll_path);
    }
    else if (inject_method == "APC") {
        InjectionFramework::APCInjector injector;
        success = injector.inject(process_name, dll_path);
    }
    else if (inject_method == "PE") {
        InjectionFramework::PEInjector injector;
        success = injector.inject(process_name, dll_path);
    }
    else {
        std::cerr << "未知的注入方式: " << inject_method << std::endl;
        return 1;
    }

    if (success) {
        std::cout << "DLL 注入成功！" << std::endl;
    }
    else {
        std::cerr << "DLL 注入失败。" << std::endl;
    }

    return success ? 0 : 1;
}
