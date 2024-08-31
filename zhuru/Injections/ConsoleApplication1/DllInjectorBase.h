// DllInjectorBase.h

#ifndef DLLINJECTORBASE_H
#define DLLINJECTORBASE_H

#include <Windows.h>
#include <string>

namespace InjectionFramework {

    class DllInjectorBase {
    public:
        DllInjectorBase() = default;
        virtual ~DllInjectorBase() = default;
        virtual bool inject(const std::string& process_name, const std::string& dll_path) = 0;

    public:
        void* get_process_handle(const std::string& process_name);
    };

} // namespace InjectionFramework

#endif // DLLINJECTORBASE_H
