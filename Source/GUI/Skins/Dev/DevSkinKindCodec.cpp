#include "GUI/Skins/Dev/DevSkinKindCodec.h"

namespace TSS::DevSkin
{
    juce::String kindToString(SkinKind kind)
    {
        switch (kind)
        {
            case SkinKind::kCompiledBlack:        return "compiledBlack";
            case SkinKind::kCompiledBlackChroma:  return "compiledBlackChroma";
            case SkinKind::kCompiledBlackMinimal: return "compiledBlackMinimal";
            case SkinKind::kCompiledCream:        return "compiledCream";
            case SkinKind::kCompiledCreamMinimal: return "compiledCreamMinimal";
            case SkinKind::kCompiledCreamChroma:  return "compiledCreamChroma";
            case SkinKind::kDev:                  return "dev";
        }
        return "dev";
    }

    bool kindFromString(const juce::String& text, SkinKind& out)
    {
        if (text == "compiledBlack")
        {
            out = SkinKind::kCompiledBlack;
            return true;
        }
        if (text == "compiledBlackChroma" || text == "compiledBlackDuo")
        {
            out = SkinKind::kCompiledBlackChroma;
            return true;
        }
        if (text == "compiledBlackMinimal")
        {
            out = SkinKind::kCompiledBlackMinimal;
            return true;
        }
        if (text == "compiledCream")
        {
            out = SkinKind::kCompiledCream;
            return true;
        }
        if (text == "compiledCreamMinimal")
        {
            out = SkinKind::kCompiledCreamMinimal;
            return true;
        }
        if (text == "compiledCreamChroma")
        {
            out = SkinKind::kCompiledCreamChroma;
            return true;
        }
        if (text == "dev")
        {
            out = SkinKind::kDev;
            return true;
        }
        return false;
    }

    bool isReservedCompiledSkinId(const juce::String& skinId) noexcept
    {
        return skinId == kCompiledBlackId
            || skinId == kCompiledBlackChromaId
            || skinId == kCompiledBlackMinimalId
            || skinId == kCompiledCreamId
            || skinId == kCompiledCreamMinimalId
            || skinId == kCompiledCreamChromaId;
    }
}
