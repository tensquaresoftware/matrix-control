#include "GUI/Skins/Dev/DevSkinStandalonePaintState.h"

#include "GUI/Skins/Dev/DevSkinApplier.h"

namespace TSS::DevSkin
{
    StandalonePaintState::StandalonePaintState()
    {
        black_ = Skin::create(Skin::ColourVariant::Black);
        blackChroma_ = Skin::create(Skin::ColourVariant::BlackChroma);
        blackMinimal_ = Skin::create(Skin::ColourVariant::BlackMinimal);
        cream_ = Skin::create(Skin::ColourVariant::Cream);
        creamMinimal_ = Skin::create(Skin::ColourVariant::CreamMinimal);
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
        else if (variant == Skin::ColourVariant::BlackChroma)
            active_ = blackChroma_.get();
        else if (variant == Skin::ColourVariant::BlackMinimal)
            active_ = blackMinimal_.get();
        else if (variant == Skin::ColourVariant::CreamMinimal)
            active_ = creamMinimal_.get();
        else
            active_ = cream_.get();
    }

    ApplyResult StandalonePaintState::applyPush(const PushPayload& payload)
    {
        auto compiledOk = [&](Skin::ColourVariant variant, const char* message)
        {
            selectCompiled(variant);
            ApplyResult result;
            result.ok = true;
            result.revision = payload.revision;
            result.appliedSkinId = payload.activeSkinId;
            result.message = message;
            return result;
        };

        if (payload.kind == SkinKind::kCompiledBlack)
            return compiledOk(Skin::ColourVariant::Black, "Compiled Black");
        if (payload.kind == SkinKind::kCompiledBlackChroma)
            return compiledOk(Skin::ColourVariant::BlackChroma, "Compiled BLACK/CHROMA");
        if (payload.kind == SkinKind::kCompiledBlackMinimal)
            return compiledOk(Skin::ColourVariant::BlackMinimal, "Compiled BLACK/MINIMAL");
        if (payload.kind == SkinKind::kCompiledCream)
            return compiledOk(Skin::ColourVariant::Cream, "Compiled Cream");
        if (payload.kind == SkinKind::kCompiledCreamMinimal)
            return compiledOk(Skin::ColourVariant::CreamMinimal, "Compiled CREAM/MINIMAL");

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
