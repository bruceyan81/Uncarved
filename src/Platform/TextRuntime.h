#pragma once

#include <memory>

namespace Uncarved::PlatformSpace
{
    class TextRuntime final
    {
    public:
        TextRuntime();

        TextRuntime(const TextRuntime&) = delete;
        TextRuntime& operator=(const TextRuntime&) = delete;

        TextRuntime(TextRuntime&&) = delete;
        TextRuntime& operator=(TextRuntime&&) = delete;

        ~TextRuntime();

        bool initialize();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
