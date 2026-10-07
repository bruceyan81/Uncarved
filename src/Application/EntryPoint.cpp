#include "EntryPoint.h"
#include "Application.h"
#include "ApplicationCore.h"

#include "Logging/Log.h"

#include <iostream>
#include <memory>
#include <utility>

namespace Uncarved::ApplicationSpace
{
    int runEntryPoint()
    {
        if (!Log::initialize())
        {
            std::cerr << "Log initialization failure.";
            return 1;
        }

        UC_LOG(gLogCore, LogVerbosity::Info, "Log initialized");

        int runResult = 1;

        {
            auto application = createApplication();

            if (application == nullptr)
            {
                UC_LOG(gApplicationCore, LogVerbosity::Error, "Application creation failure");
            }
            else
            {
                ApplicationCore applicationCore{std::move(application)};
                runResult = applicationCore.launch();
            }
        }

        Log::shutdown();

        return runResult;
    }
} // namespace Uncarved::ApplicationSpace
