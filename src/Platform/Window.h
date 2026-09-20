#pragma once

#include <iosfwd>
#include <memory>

namespace Uncarved::PlatformSpace
{
    class Renderer;

    struct WindowSize final
    {
        int width_{};
        int height_{};
    };

    class Window final
    {
    public:
        Window(WindowSize&& windowSize, std::string&& windowTitle);

        ~Window();

        bool initialize();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        friend class Renderer;
    };
} // namespace Uncarved::PlatformSpace
