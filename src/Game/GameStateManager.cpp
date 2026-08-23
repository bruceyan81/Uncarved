#include "GameStateManager.h"

#include <optional>

namespace Uncarved::GameSpace
{
    void GameStateManager::createRequest(DialogueCommand intention, std::string& nextSceneName)
    {
        if (request_ != std::nullopt)
        {
            return;
        }

        request_ = GameStateRequest{intention, std::move(nextSceneName)};
    }

} // namespace Uncarved::GameSpace
