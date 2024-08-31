// ×¢ÈëµÄÀà
#ifndef CREATE_REMOTE_THREAD_INJECTOR_H
#define CREATE_REMOTE_THREAD_INJECTOR_H

#include "DllInjectorBase.h"
#include <windows.h>

namespace InjectionFramework {

    class CreateRemoteThreadInjector : public DllInjectorBase {
    private:
        HANDLE get_process_handle(const std::string& process_name); 

    public:
        bool inject(const std::string& process_name, const std::string& dll_path) override;
    };

}

#endif // CREATE_REMOTE_THREAD_INJECTOR_H
