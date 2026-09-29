#pragma once

#include <glm/glm.hpp>

namespace Uncarved::ObjectSpace
{
    /**
     * @brief Represents a transform from an actor's local space to 2D world space.
     *
     * World space is the shared 2D coordinate space of a world.
     * Its positive X axis points right and its positive Y axis points down.
     *
     * A world unit (WU) is the abstract unit of distance in world space and has no inherent pixel or physical size.
     *
     * The transform applies scale along the actor's local axes,
     * then rotation about the local origin, followed by translation into world space.
     *
     * @member position
     * Position of the actor's local origin in world space, expressed in world units.
     *
     * @member scale
     * Dimensionless scale factors applied along the actor's local X and Y axes.
     * A negative component reverses the corresponding local axis.
     *
     * @member rotationRadians
     * Rotation of the actor about its local origin, expressed in radians.
     * Positive values represent counter-clockwise rotation.
     */
    struct Transform2D final
    {
        glm::fvec2 position_{0.0f, 0.0f};
        glm::fvec2 scale_{1.0f, 1.0f};
        float      rotationRadians_{0};
    };
} // namespace Uncarved::ObjectSpace
