#pragma once

#include <memory>

#include "GUI/Skins/Dev/DevSkinTypes.h"
#include "GUI/Skins/Skin.h"

namespace TSS::DevSkin
{
    /**
     * Standalone Dev paint path without PluginEditor: compiled Black/Cream plus optional Dev overlay.
     * Used by unit tests to prove apply/repaint and disconnect fallback.
     */
    class StandalonePaintState
    {
    public:
        StandalonePaintState();

        Skin& activeSkin() noexcept;
        const Skin& activeSkin() const noexcept;
        bool hasDevOverlay() const noexcept { return overlay_ != nullptr; }
        Skin::ColourVariant compiledSelection() const noexcept { return compiledSelection_; }

        ApplyResult applyPush(const PushPayload& payload);

        /** Disconnect / Standalone quit: drop Dev overlay; restore compiled Black/Cream. */
        void clearOverlayToCompiled();

    private:
        void selectCompiled(Skin::ColourVariant variant);

        std::unique_ptr<Skin> black_;
        std::unique_ptr<Skin> cream_;
        std::unique_ptr<Skin> overlay_;
        Skin* active_ = nullptr;
        Skin::ColourVariant compiledSelection_ = Skin::ColourVariant::Cream;
    };
}
