#include "pch.h"
#include <Windows.h>

// 全局变量
HHOOK g_hook_handle = NULL;  // 钩子句柄
HINSTANCE g_instance = NULL; // DLL实例句柄

// 钩子回调函数：例如键盘钩子
LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) { // 确保钩子有效
        // 在此处理键盘事件，wParam 是事件类型，lParam 是键盘事件结构
        MessageBoxA(NULL, "检测到键盘事件！", "钩子通知", MB_OK);
    }
    // 调用下一个钩子
    return CallNextHookEx(g_hook_handle, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // 保留原有的 CreateRemoteThread 注入弹窗
        MessageBoxA(NULL, "DLL 注入成功!", "DLL 注入", MB_OK);

        // 设置全局钩子
        g_instance = hModule; // 保存 DLL 实例句柄
        g_hook_handle = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, g_instance, 0);
        if (!g_hook_handle) {
            MessageBoxA(NULL, "设置钩子失败！", "DLL 注入", MB_OK);
        }
        else {
            MessageBoxA(NULL, "DLL 注入成功并设置钩子！", "DLL 注入", MB_OK);
        }
        break;

    case DLL_PROCESS_DETACH:
        // 保留卸载时的弹窗
        MessageBoxA(NULL, "DLL 卸载", "DLL 注入", MB_OK);

        // 卸载钩子
        if (g_hook_handle) {
            UnhookWindowsHookEx(g_hook_handle);
            g_hook_handle = NULL; // 清空钩子句柄
        }
        break;
    }
    return TRUE;
}
