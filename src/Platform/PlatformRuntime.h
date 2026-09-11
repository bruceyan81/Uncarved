#pragma once

#include <memory>

namespace Uncarved::PlatformSpace
{
    class PlatformRuntime final
    {
    public:
        PlatformRuntime();

        ~PlatformRuntime();

        bool initialize();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
