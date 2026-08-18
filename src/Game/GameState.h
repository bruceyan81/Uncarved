#pragma once

#include "GameCommon.h"
#include "Gameplay.h"

#include "Object/Actor.h"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Uncarved
{
    namespace
    {
        void updateOccupancyCount(std::size_t& currentCount, int delta)
        {
            if (delta < 0)
            {
                const std::size_t decrement = static_cast<std::size_t>(-delta);
                currentCount = decrement > currentCount ? std::size_t{0} : currentCount - decrement;
                return;
            }

            const std::size_t increment = static_cast<std::size_t>(delta);

            currentCount = increment > (std::numeric_limits<std::size_t>::max() - currentCount)
                ? std::numeric_limits<std::size_t>::max()
                : currentCount + increment;
        }
    } // namespace
} // namespace Uncarved

namespace Uncarved::GameSpace
{
    struct GameStateRequest
    {
        DialogueCommand intention_;
        std::string     sceneName_;
    };

    class GameState final
    {
    public:
        GameState() = default;

        GameState(const GameState&) = delete;
        GameState& operator=(const GameState&) = delete;

        GameState(GameState&&) noexcept = default;
        GameState& operator=(GameState&&) noexcept = default;

        int getHealth() const noexcept
        {
            return health_;
        }

        void updateHealth(int health) noexcept
        {
            health_ = health;
        }

        int getScore() const noexcept
        {
            return score_;
        }

        void updateScore(int score) noexcept
        {
            score_ = score;
        }

        std::vector<ObjectSpace::Actor>& getActors()
        {
            return actors_;
        }

        const std::vector<ObjectSpace::Actor>& getActorsReadOnly() const noexcept
        {
            return actors_;
        }

        const ObjectSpace::Actor* getPlayer() const noexcept
        {
            if (playerIndex_)
            {
                return &actors_[playerIndex_.value()];
            }
            else
            {
                return nullptr;
            }
        }

        void movePlayerBy(const glm::ivec2& delta) noexcept
        {
            if (playerIndex_)
            {
                actors_[playerIndex_.value()].moveBy(delta);
            }
        }

        std::optional<char> getWorldViewByCoord(const glm::ivec2& target) const noexcept;

        void updateWorld();

        void createRequest(DialogueCommand intention, std::string& nextSceneName);

        const std::optional<GameStateRequest>& getGameStateRequest() const noexcept
        {
            return request_;
        }

        const std::array<std::size_t, kMapSize>& getNpcOccupancyGrid() const noexcept
        {
            return npcOccupancyGrid_;
        }

        void updateNpcOccupancyGrid(std::size_t index, int delta)
        {
            std::size_t& currentCount = npcOccupancyGrid_[index];

            updateOccupancyCount(currentCount, delta);
        }

        const std::array<std::size_t, kMapSize>& getBlockingOccupancyGrid() const noexcept
        {
            return blockingOccupancyGrid_;
        }

        void updateBlockingOccupancyGrid(std::size_t index, int delta)
        {
            std::size_t& currentCount = blockingOccupancyGrid_[index];

            updateOccupancyCount(currentCount, delta);
        }

        ObjectSpace::Actor* getActorById(ObjectSpace::ActorId id)
        {
            const auto it = actorIndexById_.find(id);

            if (it == actorIndexById_.end())
            {
                return nullptr;
            }

            return &actors_[it->second];
        }

        ~GameState() = default;

    private:
        int                                                   health_{};
        int                                                   score_{};
        std::optional<std::size_t>                            playerIndex_{};
        std::vector<ObjectSpace::Actor>                       actors_{};
        std::unordered_map<ObjectSpace::ActorId, std::size_t> actorIndexById_{};
        std::array<char, kMapSize>                            worldBuffer_{};
        std::array<std::size_t, kMapSize>                     npcOccupancyGrid_{};
        std::array<std::size_t, kMapSize>                     blockingOccupancyGrid_{};
        std::optional<GameStateRequest>                       request_{std::nullopt};

        friend class GameCore;
    };
} // namespace Uncarved::GameSpace
