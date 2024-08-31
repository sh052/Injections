#pragma once
#include <string>
#include "DllInjectorBase.h"

namespace InjectionFramework {

    class SetWindowsHookExInjector : public DllInjectorBase {
    public:
        bool inject(const std::string& process_name, const std::string& dll_path) override;
    };

} // namespace InjectionFramework
