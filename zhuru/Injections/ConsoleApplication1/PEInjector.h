#pragma once
#include "DllInjectorBase.h"

namespace InjectionFramework {
    class PEInjector : public DllInjectorBase {
    public:
        bool inject(const std::string& process_name, const std::string& dll_path) override;
    };
}
