#pragma once

#include <iosfwd>
#include <memory>

namespace Uncarved
{
    class Font;

    namespace ContentSpace
    {
        struct ContentResult;
    }
} // namespace Uncarved

namespace Uncarved::PlatformSpace
{
    class TextRuntime;

    class FontLoader final
    {
    public:
        explicit FontLoader(TextRuntime& outTextRuntime);

        ~FontLoader();

        ContentSpace::ContentResult loadFont(const std::string& fontPath);

        Font& getFont() noexcept;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
