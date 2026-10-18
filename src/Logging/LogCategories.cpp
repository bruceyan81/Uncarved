#include "LogCategories.h"

namespace Uncarved
{
    const LogCategory gLogCore{"LogCore", LogVerbosity::Trace};

    const LogCategory gApplicationCore{"ApplicationCore", LogVerbosity::Trace};

    const LogCategory gPlatform{"Platform", LogVerbosity::Trace};

    const LogCategory gContent{"Content", LogVerbosity::Trace};

    const LogCategory gRuntime{"Runtime", LogVerbosity::Trace};

    const LogCategory gRender{"Render", LogVerbosity::Trace};

    const LogCategory gAudio{"Audio", LogVerbosity::Trace};
} // namespace Uncarved
