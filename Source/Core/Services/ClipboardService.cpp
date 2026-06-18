#include "ClipboardService.h"

#include <cstring>
#include <optional>

#include "Core/Models/PackedFieldCodec.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

namespace
{

    struct ModuleDescriptorViews
    {
        const std::vector<PluginDescriptors::IntParameterDescriptor>& intParams;
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& choiceParams;
    };

    ModuleDescriptorViews descriptorsFor(PatchModuleKind kind)
    {
        using namespace PluginDescriptors::PatchEditSection;

        switch (kind)
        {
            case PatchModuleKind::Dco1: return { Dco1Module::kIntParameters, Dco1Module::kChoiceParameters };
            case PatchModuleKind::Dco2: return { Dco2Module::kIntParameters, Dco2Module::kChoiceParameters };
            case PatchModuleKind::Env1: return { Envelope1Module::kIntParameters, Envelope1Module::kChoiceParameters };
            case PatchModuleKind::Env2: return { Envelope2Module::kIntParameters, Envelope2Module::kChoiceParameters };
            case PatchModuleKind::Env3: return { Envelope3Module::kIntParameters, Envelope3Module::kChoiceParameters };
            case PatchModuleKind::Lfo1: return { Lfo1Module::kIntParameters, Lfo1Module::kChoiceParameters };
            case PatchModuleKind::Lfo2: return { Lfo2Module::kIntParameters, Lfo2Module::kChoiceParameters };
        }

        return { Dco1Module::kIntParameters, Dco1Module::kChoiceParameters };
    }

    const PluginDescriptors::IntParameterDescriptor* findIntByDisplayName(
        const std::vector<PluginDescriptors::IntParameterDescriptor>& params,
        const juce::String& displayName)
    {
        for (const auto& descriptor : params)
        {
            if (descriptor.displayName == displayName)
                return &descriptor;
        }

        return nullptr;
    }

    const PluginDescriptors::ChoiceParameterDescriptor* findChoiceByDisplayName(
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& params,
        const juce::String& displayName)
    {
        for (const auto& descriptor : params)
        {
            if (descriptor.displayName == displayName)
                return &descriptor;
        }

        return nullptr;
    }

    bool shouldSkipDcoIntParam(PatchModuleKind source, PatchModuleKind target, const juce::String& paramId)
    {
        using namespace PluginIDs::PatchEditSection;

        if (source == target)
            return false;

        if (paramId == Dco2Module::ParameterWidgets::kDetune)
            return true;

        juce::ignoreUnused(source, target);
        return false;
    }

    bool shouldSkipDcoChoiceParam(PatchModuleKind source, PatchModuleKind target, const juce::String& paramId)
    {
        using namespace PluginIDs::PatchEditSection;

        if (source == target)
            return false;

        if (paramId == Dco1Module::ParameterWidgets::kSync)
            return true;

        juce::ignoreUnused(source, target);
        return false;
    }

    std::optional<int> mapDcoWaveSelectIndex(PatchModuleKind source,
                                             PatchModuleKind target,
                                             int sourceIndex)
    {
        if (source == target)
            return sourceIndex;

        if (source == PatchModuleKind::Dco2 && target == PatchModuleKind::Dco1 && sourceIndex == 4)
            return std::nullopt;

        if (sourceIndex >= 0 && sourceIndex <= 3)
            return sourceIndex;

        return std::nullopt;
    }

    std::optional<int> mapDcoKeyboardPortamentoIndex(PatchModuleKind source,
                                                     PatchModuleKind target,
                                                     int sourceIndex)
    {
        if (source == target)
            return sourceIndex;

        if (source == PatchModuleKind::Dco2 && target == PatchModuleKind::Dco1)
        {
            if (sourceIndex == 0)
                return std::nullopt;

            if (sourceIndex == 1)
                return 1;

            if (sourceIndex == 2)
                return 0;
        }
        else if (source == PatchModuleKind::Dco1 && target == PatchModuleKind::Dco2)
        {
            if (sourceIndex == 0)
                return 2;

            if (sourceIndex == 1)
                return 1;
        }

        return std::nullopt;
    }

    juce::String resolveLfoSourceIntParamId(PatchModuleKind source,
                                            PatchModuleKind target,
                                            const juce::String& targetParamId)
    {
        using namespace PluginIDs::PatchEditSection;

        if (source == PatchModuleKind::Lfo1 && target == PatchModuleKind::Lfo2)
        {
            if (targetParamId == Lfo2Module::ParameterWidgets::kSpeedModByKeyboard)
                return Lfo1Module::ParameterWidgets::kSpeedModByPressure;

            if (targetParamId == Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2)
                return Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1;
        }
        else if (source == PatchModuleKind::Lfo2 && target == PatchModuleKind::Lfo1)
        {
            if (targetParamId == Lfo1Module::ParameterWidgets::kSpeedModByPressure)
                return Lfo2Module::ParameterWidgets::kSpeedModByKeyboard;

            if (targetParamId == Lfo1Module::ParameterWidgets::kAmplitudeModByRamp1)
                return Lfo2Module::ParameterWidgets::kAmplitudeModByRamp2;
        }

        return targetParamId;
    }

} // namespace

bool ClipboardService::hasContent() const noexcept
{
    return mode_ != ClipboardMode::Empty;
}

ClipboardMode ClipboardService::getMode() const noexcept
{
    return mode_;
}

std::optional<PatchModuleKind> ClipboardService::getSourceModuleKind() const noexcept
{
    if (mode_ != ClipboardMode::Module)
        return std::nullopt;

    return sourceModuleKind_;
}

void ClipboardService::copyModule(PatchModuleKind source, const PatchModel& model)
{
    mode_ = ClipboardMode::Module;
    sourceModuleKind_ = source;
    moduleSnapshot_.intValues.clear();
    moduleSnapshot_.choiceIndices.clear();

    const auto views = descriptorsFor(source);

    for (const auto& descriptor : views.intParams)
        moduleSnapshot_.intValues[descriptor.parameterId] = model.getValue(descriptor);

    for (const auto& descriptor : views.choiceParams)
        moduleSnapshot_.choiceIndices[descriptor.parameterId] = model.getChoiceIndex(descriptor);
}

void ClipboardService::copyFullPatch(const PatchModel& model)
{
    mode_ = ClipboardMode::FullPatch;
    std::memcpy(fullPatchSnapshot_.data(), model.data(), fullPatchSnapshot_.size());
}

void ClipboardService::copyMatrixModulation(const PatchModel& model)
{
    mode_ = ClipboardMode::MatrixModulation;

    const auto offset = PackedFieldCodec::safeOffset(
        static_cast<int>(kMatrixModSnapshotOffset),
        PatchModel::kBufferSize);

    std::memcpy(matrixModSnapshot_.data(), model.data() + offset, kMatrixModSnapshotSize);
}

bool ClipboardService::canPasteModule(PatchModuleKind target) const noexcept
{
    if (mode_ != ClipboardMode::Module)
        return false;

    return areModulesCompatible(sourceModuleKind_, target);
}

bool ClipboardService::canPasteFullPatch() const noexcept
{
    return mode_ == ClipboardMode::FullPatch;
}

bool ClipboardService::canPasteMatrixModulation() const noexcept
{
    return mode_ == ClipboardMode::MatrixModulation;
}

bool ClipboardService::pasteModule(PatchModuleKind target, PatchModel& model)
{
    if (!canPasteModule(target))
        return false;

    if (sourceModuleKind_ == target)
        pasteModuleDirect(target, model);
    else if (isEnvelopeModule(sourceModuleKind_) && isEnvelopeModule(target))
        pasteEnvelopeModule(sourceModuleKind_, target, model);
    else if (isDcoModule(sourceModuleKind_) && isDcoModule(target))
        pasteDcoModule(sourceModuleKind_, target, model);
    else if (isLfoModule(sourceModuleKind_) && isLfoModule(target))
        pasteLfoModule(sourceModuleKind_, target, model);

    return true;
}

bool ClipboardService::pasteFullPatch(PatchModel& model)
{
    if (!canPasteFullPatch())
        return false;

    model.loadFrom(fullPatchSnapshot_.data());
    return true;
}

bool ClipboardService::pasteMatrixModulation(PatchModel& model)
{
    if (!canPasteMatrixModulation())
        return false;

    const auto offset = PackedFieldCodec::safeOffset(
        static_cast<int>(kMatrixModSnapshotOffset),
        PatchModel::kBufferSize);

    std::memcpy(model.data() + offset, matrixModSnapshot_.data(), kMatrixModSnapshotSize);
    return true;
}

bool ClipboardService::isEnvelopeModule(PatchModuleKind kind) noexcept
{
    return kind == PatchModuleKind::Env1 || kind == PatchModuleKind::Env2 || kind == PatchModuleKind::Env3;
}

bool ClipboardService::isDcoModule(PatchModuleKind kind) noexcept
{
    return kind == PatchModuleKind::Dco1 || kind == PatchModuleKind::Dco2;
}

bool ClipboardService::isLfoModule(PatchModuleKind kind) noexcept
{
    return kind == PatchModuleKind::Lfo1 || kind == PatchModuleKind::Lfo2;
}

bool ClipboardService::areModulesCompatible(PatchModuleKind source, PatchModuleKind target) noexcept
{
    if (isEnvelopeModule(source) && isEnvelopeModule(target))
        return true;

    if (isDcoModule(source) && isDcoModule(target))
        return true;

    if (isLfoModule(source) && isLfoModule(target))
        return true;

    return false;
}

void ClipboardService::pasteModuleDirect(PatchModuleKind target, PatchModel& model)
{
    const auto views = descriptorsFor(target);

    for (const auto& descriptor : views.intParams)
    {
        const auto it = moduleSnapshot_.intValues.find(descriptor.parameterId);
        if (it != moduleSnapshot_.intValues.end())
            model.setValue(descriptor, it->second);
    }

    for (const auto& descriptor : views.choiceParams)
    {
        const auto it = moduleSnapshot_.choiceIndices.find(descriptor.parameterId);
        if (it != moduleSnapshot_.choiceIndices.end())
            model.setChoiceIndex(descriptor, it->second);
    }
}

void ClipboardService::pasteEnvelopeModule(PatchModuleKind source,
                                           PatchModuleKind target,
                                           PatchModel& model)
{
    const auto sourceViews = descriptorsFor(source);
    const auto targetViews = descriptorsFor(target);

    for (const auto& targetDescriptor : targetViews.intParams)
    {
        const auto* sourceDescriptor = findIntByDisplayName(
            sourceViews.intParams,
            targetDescriptor.displayName);

        if (sourceDescriptor == nullptr)
            continue;

        const auto it = moduleSnapshot_.intValues.find(sourceDescriptor->parameterId);
        if (it != moduleSnapshot_.intValues.end())
            model.setValue(targetDescriptor, it->second);
    }

    for (const auto& targetDescriptor : targetViews.choiceParams)
    {
        const auto* sourceDescriptor = findChoiceByDisplayName(
            sourceViews.choiceParams,
            targetDescriptor.displayName);

        if (sourceDescriptor == nullptr)
            continue;

        const auto it = moduleSnapshot_.choiceIndices.find(sourceDescriptor->parameterId);
        if (it != moduleSnapshot_.choiceIndices.end())
            model.setChoiceIndex(targetDescriptor, it->second);
    }
}

void ClipboardService::pasteDcoModule(PatchModuleKind source, PatchModuleKind target, PatchModel& model)
{
    using namespace PluginIDs::PatchEditSection;

    const auto sourceViews = descriptorsFor(source);
    const auto targetViews = descriptorsFor(target);

    for (const auto& targetDescriptor : targetViews.intParams)
    {
        if (shouldSkipDcoIntParam(source, target, targetDescriptor.parameterId))
            continue;

        const auto* sourceDescriptor = findIntByDisplayName(
            sourceViews.intParams,
            targetDescriptor.displayName);

        if (sourceDescriptor == nullptr)
            continue;

        const auto it = moduleSnapshot_.intValues.find(sourceDescriptor->parameterId);
        if (it != moduleSnapshot_.intValues.end())
            model.setValue(targetDescriptor, it->second);
    }

    for (const auto& targetDescriptor : targetViews.choiceParams)
    {
        if (shouldSkipDcoChoiceParam(source, target, targetDescriptor.parameterId))
            continue;

        const auto* sourceDescriptor = findChoiceByDisplayName(
            sourceViews.choiceParams,
            targetDescriptor.displayName);

        if (sourceDescriptor == nullptr)
            continue;

        const auto it = moduleSnapshot_.choiceIndices.find(sourceDescriptor->parameterId);
        if (it == moduleSnapshot_.choiceIndices.end())
            continue;

        if (targetDescriptor.parameterId == Dco1Module::ParameterWidgets::kWaveSelect
            || targetDescriptor.parameterId == Dco2Module::ParameterWidgets::kWaveSelect)
        {
            const auto mapped = mapDcoWaveSelectIndex(source, target, it->second);
            if (!mapped.has_value())
                continue;

            model.setChoiceIndex(targetDescriptor, *mapped);
            continue;
        }

        if (targetDescriptor.parameterId == Dco1Module::ParameterWidgets::kKeyboardPortamento
            || targetDescriptor.parameterId == Dco2Module::ParameterWidgets::kKeyboardPortamento)
        {
            const auto mapped = mapDcoKeyboardPortamentoIndex(source, target, it->second);
            if (!mapped.has_value())
                continue;

            model.setChoiceIndex(targetDescriptor, *mapped);
            continue;
        }

        model.setChoiceIndex(targetDescriptor, it->second);
    }
}

void ClipboardService::pasteLfoModule(PatchModuleKind source, PatchModuleKind target, PatchModel& model)
{
    const auto sourceViews = descriptorsFor(source);
    const auto targetViews = descriptorsFor(target);

    for (const auto& targetDescriptor : targetViews.intParams)
    {
        const auto sourceParamId = resolveLfoSourceIntParamId(
            source,
            target,
            targetDescriptor.parameterId);

        auto it = moduleSnapshot_.intValues.find(sourceParamId);

        if (it == moduleSnapshot_.intValues.end() && source != target)
        {
            const auto* sourceDescriptor = findIntByDisplayName(
                sourceViews.intParams,
                targetDescriptor.displayName);

            if (sourceDescriptor != nullptr)
                it = moduleSnapshot_.intValues.find(sourceDescriptor->parameterId);
        }

        if (it != moduleSnapshot_.intValues.end())
            model.setValue(targetDescriptor, it->second);
    }

    for (const auto& targetDescriptor : targetViews.choiceParams)
    {
        const auto* sourceDescriptor = findChoiceByDisplayName(
            sourceViews.choiceParams,
            targetDescriptor.displayName);

        if (sourceDescriptor == nullptr)
            continue;

        const auto it = moduleSnapshot_.choiceIndices.find(sourceDescriptor->parameterId);
        if (it != moduleSnapshot_.choiceIndices.end())
            model.setChoiceIndex(targetDescriptor, it->second);
    }
}

} // namespace Core
