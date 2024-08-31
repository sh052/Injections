#include <iostream>
#include <Windows.h>

int main() {
    std::cout << "目标应用程序正在运行。。。" << std::endl;
    while (true) {
        Sleep(1000);  // 每秒输出一次，保持运行状态
        std::cout << "程序运行" << std::endl;
    }
    return 0;
}

