#pragma once

#include <glm/glm.hpp>

#include <optional>

namespace Uncarved::ViewSpace
{
    class Camera2D final
    {
    public:
        static std::optional<Camera2D> createCamera2D(float orthoWidth) noexcept;

        Camera2D(const Camera2D&) = delete;
        Camera2D& operator=(const Camera2D&) = delete;

        Camera2D(Camera2D&&) noexcept = default;
        Camera2D& operator=(Camera2D&&) noexcept = default;

        ~Camera2D() = default;

        const glm::fvec2& getPosition() const noexcept;
        void              setPosition(const glm::fvec2& position) noexcept;

        float getOrthoWidth() const noexcept;
        bool  trySetOrthoWidth(float orthoWidth) noexcept;

        void moveBy(const glm::fvec2& delta) noexcept;

        float getViewportPixelsPerWorldUnit(int viewportWidthPixels) const noexcept;

        /**
         * @brief ワールド空間上の座標をビューポートのピクセル座標へ変換する
         * @param worldPosition 単位は WU，ワールド原点を基準としたワールド空間上の座標
         * @param viewportSizePixels 単位は pixel，カメラの描画先となるビューポートのサイズ
         * @return 単位は pixel，ビューポート左上を原点とするピクセル座標
         */
        glm::fvec2 projectWorldToViewport(
            const glm::fvec2& worldPosition, const glm::ivec2& viewportSizePixels
        ) const noexcept;

    private:
        explicit Camera2D(float orthoWidth);

        glm::fvec2 position_{};
        float      orthoWidth_;
    };
} // namespace Uncarved::ViewSpace
