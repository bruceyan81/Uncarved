#include "GameState.h"

#include <optional>

namespace Uncarved::GameSpace
{
    std::optional<char> GameState::getWorldViewByCoord(const glm::ivec2& target) const noexcept
    {
        if (target.x >= 0 && target.x < kMapWidth && target.y >= 0 && target.y < kMapHeight)
        {
            return worldBuffer_[target.y * kMapWidth + target.x];
        }
        return std::nullopt;
    }

    void GameState::updateWorld()
    {
        this->worldBuffer_.fill(' ');

        for (const auto& actor : this->actors_)
        {
            const glm::ivec2 position = actor.getPosition();

            if (position.x < 0 || position.x >= kMapWidth || position.y < 0 || position.y >= kMapHeight)
            {
                continue;
            }

            this->worldBuffer_[position.y * kMapWidth + position.x] = actor.getActorView();
        }
    }

    void GameState::createRequest(DialogueCommand intention, std::string& nextSceneName)
    {
        if (request_ != std::nullopt)
        {
            return;
        }

        request_ = GameStateRequest{intention, std::move(nextSceneName)};
    }

} // namespace Uncarved::GameSpace
