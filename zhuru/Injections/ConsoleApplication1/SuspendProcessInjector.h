#ifndef SUSPEND_PROCESS_INJECTOR_H
#define SUSPEND_PROCESS_INJECTOR_H

#include <Windows.h>
#include <string>
#include "DllInjectorBase.h"

namespace InjectionFramework {

    class SuspendProcessInjector : public DllInjectorBase {
    public:
        SuspendProcessInjector() = default;
        ~SuspendProcessInjector() override = default;

        //DLL ×¢Èë
        bool inject(const std::string& process_name, const std::string& dll_path) override;

    private:
        bool suspend_process_injection(HANDLE process_handle, const std::string& dll_path);
        void suspend_all_threads(HANDLE process_handle);
        void resume_all_threads(HANDLE process_handle);

        bool suspend_threads(DWORD process_id);
        bool resume_threads(DWORD process_id);
        HANDLE get_process_handle(const std::string& process_name);
    };

} // namespace InjectionFramework

#endif // SUSPEND_PROCESS_INJECTOR_H
