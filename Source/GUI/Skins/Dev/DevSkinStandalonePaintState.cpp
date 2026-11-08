#include "GUI/Skins/Dev/DevSkinStandalonePaintState.h"

#include "GUI/Skins/Dev/DevSkinApplier.h"

namespace TSS::DevSkin
{
    StandalonePaintState::StandalonePaintState()
    {
        black_ = Skin::create(Skin::ColourVariant::Black);
        blackDuo_ = Skin::create(Skin::ColourVariant::BlackDuo);
        cream_ = Skin::create(Skin::ColourVariant::Cream);
        selectCompiled(Skin::ColourVariant::Cream);
    }

    Skin& StandalonePaintState::activeSkin() noexcept
    {
        jassert(active_ != nullptr);
        return *active_;
    }

    const Skin& StandalonePaintState::activeSkin() const noexcept
    {
        jassert(active_ != nullptr);
        return *active_;
    }

    void StandalonePaintState::selectCompiled(Skin::ColourVariant variant)
    {
        overlay_.reset();
        compiledSelection_ = variant;
        if (variant == Skin::ColourVariant::Black)
            active_ = black_.get();
        else if (variant == Skin::ColourVariant::BlackDuo)
            active_ = blackDuo_.get();
        else
            active_ = cream_.get();
    }

    ApplyResult StandalonePaintState::applyPush(const PushPayload& payload)
    {
        if (payload.kind == SkinKind::kCompiledBlack)
        {
            selectCompiled(Skin::ColourVariant::Black);
            ApplyResult result;
            result.ok = true;
            result.revision = payload.revision;
            result.appliedSkinId = payload.activeSkinId;
            result.message = "Compiled Black";
            return result;
        }

        if (payload.kind == SkinKind::kCompiledBlackDuo)
        {
            selectCompiled(Skin::ColourVariant::BlackDuo);
            ApplyResult result;
            result.ok = true;
            result.revision = payload.revision;
            result.appliedSkinId = payload.activeSkinId;
            result.message = "Compiled Black/Duo";
            return result;
        }

        if (payload.kind == SkinKind::kCompiledCream)
        {
            selectCompiled(Skin::ColourVariant::Cream);
            ApplyResult result;
            result.ok = true;
            result.revision = payload.revision;
            result.appliedSkinId = payload.activeSkinId;
            result.message = "Compiled Cream";
            return result;
        }

        const auto base = compiledBaseVariant(payload);
        auto nextOverlay = Skin::create(base);
        const auto applied = applyPushToSkin(payload, *nextOverlay);
        if (! applied.ok)
            return applied;

        overlay_ = std::move(nextOverlay);
        active_ = overlay_.get();
        compiledSelection_ = base;
        return applied;
    }

    void StandalonePaintState::clearOverlayToCompiled()
    {
        selectCompiled(compiledSelection_);
    }
}
