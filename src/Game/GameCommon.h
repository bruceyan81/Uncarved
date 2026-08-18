#pragma once

#include <glm/glm.hpp>

#include <cstddef>

namespace Uncarved::GameSpace
{
    constexpr std::size_t kMapWidth{50};
    constexpr std::size_t kMapHeight{26};
    constexpr std::size_t kMapSize{kMapWidth * kMapHeight};

    constexpr glm::ivec2 kMinCoordinate{0, 0};
    constexpr glm::ivec2 kMaxCoordinate{kMapWidth - 1, kMapHeight - 1};
} // namespace Uncarved::GameSpace
