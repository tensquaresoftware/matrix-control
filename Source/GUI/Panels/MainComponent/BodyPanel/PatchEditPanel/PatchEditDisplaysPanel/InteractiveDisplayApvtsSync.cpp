#include "InteractiveDisplayApvtsSync.h"

#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/PatchEditBottomModulesPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env1Panel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env2Panel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env3Panel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/PatchEditTopModulesPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/FmTrackPanel.h"

#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "GUI/Widgets/EnvelopeDisplay.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/TrackGeneratorDisplay.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    using EnvelopeBindings = std::array<InteractiveDisplayApvtsSync::EnvelopeParameterBinding,
                                        InteractiveDisplayApvtsSync::kEnvelopeParameterCount>;

    const EnvelopeBindings kEnvelope1Bindings {{
        { PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay },
        { PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack },
        { PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay },
        { PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain },
        { PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease }
    }};

    const EnvelopeBindings kEnvelope2Bindings {{
        { PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay },
        { PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack },
        { PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay },
        { PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain },
        { PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease }
    }};

    const EnvelopeBindings kEnvelope3Bindings {{
        { PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay },
        { PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack },
        { PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay },
        { PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain },
        { PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease }
    }};

    const std::array<const char*, InteractiveDisplayApvtsSync::kTrackPointCount> kTrackPointIds {{
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5
    }};

    void collectInteractiveParameterIds(std::vector<juce::String>& parameterIds)
    {
        for (const auto& bindings : { kEnvelope1Bindings, kEnvelope2Bindings, kEnvelope3Bindings })
        {
            for (const auto& binding : bindings)
                parameterIds.emplace_back(binding.parameterId);
        }

        for (const char* trackPointId : kTrackPointIds)
            parameterIds.emplace_back(trackPointId);
    }
}

class InteractiveDisplayApvtsSync::SliderToDisplayListener : public juce::Slider::Listener
{
public:
    explicit SliderToDisplayListener(std::function<void(int)> handler)
        : handler_(std::move(handler))
    {
    }

    void sliderValueChanged(juce::Slider* slider) override
    {
        if (handler_ != nullptr && slider != nullptr)
            handler_(static_cast<int>(slider->getValue()));
    }

private:
    std::function<void(int)> handler_;
};

InteractiveDisplayApvtsSync::InteractiveDisplayApvtsSync(juce::AudioProcessorValueTreeState& apvts,
                                                         TSS::EnvelopeDisplay& envelope1,
                                                         TSS::EnvelopeDisplay& envelope2,
                                                         TSS::EnvelopeDisplay& envelope3,
                                                         TSS::TrackGeneratorDisplay& trackGenerator)
    : apvts_(apvts)
    , envelope1_(envelope1)
    , envelope2_(envelope2)
    , envelope3_(envelope3)
    , trackGenerator_(trackGenerator)
{
    registerApvtsListeners();
    connectDisplayCallbacks();
}

InteractiveDisplayApvtsSync::~InteractiveDisplayApvtsSync()
{
    for (const auto& parameterId : listenedParameterIds_)
        apvts_.removeParameterListener(parameterId, this);
}

void InteractiveDisplayApvtsSync::registerApvtsListeners()
{
    collectInteractiveParameterIds(listenedParameterIds_);

    for (const auto& parameterId : listenedParameterIds_)
        apvts_.addParameterListener(parameterId, this);
}

void InteractiveDisplayApvtsSync::syncAllFromApvts()
{
    for (int paramIndex = 0; paramIndex < kEnvelopeParameterCount; ++paramIndex)
    {
        applyEnvelopeDisplayValue(
            envelope1_,
            paramIndex,
            readIntParameter(kEnvelope1Bindings[static_cast<size_t>(paramIndex)].parameterId));
        applyEnvelopeDisplayValue(
            envelope2_,
            paramIndex,
            readIntParameter(kEnvelope2Bindings[static_cast<size_t>(paramIndex)].parameterId));
        applyEnvelopeDisplayValue(
            envelope3_,
            paramIndex,
            readIntParameter(kEnvelope3Bindings[static_cast<size_t>(paramIndex)].parameterId));
    }

    for (int pointIndex = 0; pointIndex < kTrackPointCount; ++pointIndex)
        applyTrackPointDisplayValue(pointIndex, readIntParameter(kTrackPointIds[static_cast<size_t>(pointIndex)]));
}

void InteractiveDisplayApvtsSync::connectSliderFastPaths(PatchEditTopModulesPanel& topModulesPanel,
                                                         PatchEditBottomModulesPanel& bottomModulesPanel)
{
    if (auto* fmTrackPanel = topModulesPanel.getFmTrackPanel())
    {
        for (int pointIndex = 0; pointIndex < kTrackPointCount; ++pointIndex)
        {
            const size_t parameterPanelIndex = static_cast<size_t>(kTrackPointSliderStartIndex + pointIndex);

            if (auto* paramCell = fmTrackPanel->getParameterCellAt(parameterPanelIndex))
            {
                if (auto* slider = paramCell->getSlider())
                {
                    connectTrackPointSlider(
                        *slider,
                        pointIndex,
                        kTrackPointIds[static_cast<size_t>(pointIndex)]);
                }
            }
        }
    }

    BaseModulePanel* envPanels[3] {};
    envPanels[0] = bottomModulesPanel.getEnv1Panel();
    envPanels[1] = bottomModulesPanel.getEnv2Panel();
    envPanels[2] = bottomModulesPanel.getEnv3Panel();

    TSS::EnvelopeDisplay* envelopeDisplays[] { &envelope1_, &envelope2_, &envelope3_ };
    const EnvelopeBindings* envelopeBindings[] { &kEnvelope1Bindings, &kEnvelope2Bindings, &kEnvelope3Bindings };

    for (int envIndex = 0; envIndex < 3; ++envIndex)
    {
        if (envPanels[envIndex] == nullptr || envelopeDisplays[envIndex] == nullptr || envelopeBindings[envIndex] == nullptr)
            continue;

        for (int paramIndex = 0; paramIndex < kEnvelopeParameterCount; ++paramIndex)
        {
            if (auto* paramCell = envPanels[envIndex]->getParameterCellAt(static_cast<size_t>(paramIndex)))
            {
                if (auto* slider = paramCell->getSlider())
                {
                    connectEnvelopeSlider(
                        *slider,
                        *envelopeDisplays[envIndex],
                        paramIndex,
                        (*envelopeBindings[envIndex])[static_cast<size_t>(paramIndex)].parameterId);
                }
            }
        }
    }
}

bool InteractiveDisplayApvtsSync::applyApvtsChange(const juce::String& parameterId)
{
    for (int paramIndex = 0; paramIndex < kEnvelopeParameterCount; ++paramIndex)
    {
        if (parameterId == kEnvelope1Bindings[static_cast<size_t>(paramIndex)].parameterId)
        {
            applyEnvelopeDisplayValue(envelope1_, paramIndex, readIntParameter(parameterId));
            return true;
        }

        if (parameterId == kEnvelope2Bindings[static_cast<size_t>(paramIndex)].parameterId)
        {
            applyEnvelopeDisplayValue(envelope2_, paramIndex, readIntParameter(parameterId));
            return true;
        }

        if (parameterId == kEnvelope3Bindings[static_cast<size_t>(paramIndex)].parameterId)
        {
            applyEnvelopeDisplayValue(envelope3_, paramIndex, readIntParameter(parameterId));
            return true;
        }
    }

    for (int pointIndex = 0; pointIndex < kTrackPointCount; ++pointIndex)
    {
        if (parameterId != kTrackPointIds[static_cast<size_t>(pointIndex)])
            continue;

        applyTrackPointDisplayValue(pointIndex, readIntParameter(parameterId));
        return true;
    }

    return false;
}

void InteractiveDisplayApvtsSync::connectDisplayCallbacks()
{
    connectEnvelopeDisplay(envelope1_, kEnvelope1Bindings.data(), kEnvelopeParameterCount);
    connectEnvelopeDisplay(envelope2_, kEnvelope2Bindings.data(), kEnvelopeParameterCount);
    connectEnvelopeDisplay(envelope3_, kEnvelope3Bindings.data(), kEnvelopeParameterCount);
    connectTrackGeneratorDisplay();
}

void InteractiveDisplayApvtsSync::connectEnvelopeDisplay(
    TSS::EnvelopeDisplay& display,
    const EnvelopeParameterBinding* bindings,
    int bindingCount)
{
    display.setOnValueChanged([this, bindings, bindingCount](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= bindingCount)
            return;

        writeIntParameter(bindings[paramIndex].parameterId, newValue);
    });

    display.setOnEditGestureBegin([this, bindings, bindingCount](int paramIndex)
    {
        if (paramIndex < 0 || paramIndex >= bindingCount)
            return;

        beginParameterGesture(bindings[paramIndex].parameterId);
    });

    display.setOnEditGestureEnd([this]
    {
        endParameterGesture();
    });
}

void InteractiveDisplayApvtsSync::connectTrackGeneratorDisplay()
{
    trackGenerator_.setOnValueChanged([this](int pointIndex, int newValue)
    {
        if (pointIndex < 0 || pointIndex >= kTrackPointCount)
            return;

        writeIntParameter(kTrackPointIds[static_cast<size_t>(pointIndex)], newValue);
    });

    trackGenerator_.setOnEditGestureBegin([this](int pointIndex)
    {
        if (pointIndex < 0 || pointIndex >= kTrackPointCount)
            return;

        beginParameterGesture(kTrackPointIds[static_cast<size_t>(pointIndex)]);
    });

    trackGenerator_.setOnEditGestureEnd([this]
    {
        endParameterGesture();
    });
}

void InteractiveDisplayApvtsSync::connectEnvelopeSlider(
    TSS::Slider& slider,
    TSS::EnvelopeDisplay& display,
    int paramIndex,
    const juce::String& parameterId)
{
    registerSliderForParameter(slider, parameterId);

    auto listener = std::make_unique<SliderToDisplayListener>(
        [&display, paramIndex](int value)
        {
            switch (paramIndex)
            {
                case 0: display.setDelay(value, false); break;
                case 1: display.setAttack(value, false); break;
                case 2: display.setDecay(value, false); break;
                case 3: display.setSustain(value, false); break;
                case 4: display.setRelease(value, false); break;
                default: break;
            }
        });

    slider.addListener(listener.get());
    sliderListeners_.push_back(std::move(listener));
}

void InteractiveDisplayApvtsSync::connectTrackPointSlider(
    TSS::Slider& slider,
    int pointIndex,
    const juce::String& parameterId)
{
    registerSliderForParameter(slider, parameterId);

    auto listener = std::make_unique<SliderToDisplayListener>(
        [this, pointIndex](int value)
        {
            applyTrackPointDisplayValue(pointIndex, value);
        });

    slider.addListener(listener.get());
    sliderListeners_.push_back(std::move(listener));
}

int InteractiveDisplayApvtsSync::readIntParameter(const juce::String& parameterId) const
{
    auto* param = apvts_.getParameter(parameterId);
    if (param == nullptr)
        return 0;

    if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
    {
        return juce::jlimit(
            0,
            kIntParameterMax,
            juce::roundToInt(rangedParam->convertFrom0to1(param->getValue())));
    }

    return juce::jlimit(
        0,
        kIntParameterMax,
        juce::roundToInt(param->getValue() * static_cast<float>(kIntParameterMax)));
}

void InteractiveDisplayApvtsSync::writeIntParameter(const juce::String& parameterId, int value)
{
    auto* param = apvts_.getParameter(parameterId);
    if (param == nullptr)
        return;

    const int clampedValue = juce::jlimit(0, kIntParameterMax, value);

    if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
    {
        param->setValueNotifyingHost(rangedParam->convertTo0to1(static_cast<float>(clampedValue)));
    }
    else
    {
        param->setValueNotifyingHost(static_cast<float>(clampedValue) / static_cast<float>(kIntParameterMax));
    }

    updateSliderUi(parameterId, clampedValue);
}

void InteractiveDisplayApvtsSync::updateSliderUi(const juce::String& parameterId, int value)
{
    const auto iterator = slidersByParameterId_.find(parameterId);
    if (iterator == slidersByParameterId_.end() || iterator->second == nullptr)
        return;

    iterator->second->setValue(static_cast<double>(value), juce::dontSendNotification);
}

void InteractiveDisplayApvtsSync::registerSliderForParameter(
    TSS::Slider& slider,
    const juce::String& parameterId)
{
    slidersByParameterId_[parameterId] = &slider;
}

void InteractiveDisplayApvtsSync::beginParameterGesture(const juce::String& parameterId)
{
    endParameterGesture();

    if (auto* param = apvts_.getParameter(parameterId))
    {
        param->beginChangeGesture();
        activeGestureParameter_ = param;
    }
}

void InteractiveDisplayApvtsSync::endParameterGesture()
{
    if (activeGestureParameter_ == nullptr)
        return;

    activeGestureParameter_->endChangeGesture();
    activeGestureParameter_ = nullptr;
}

void InteractiveDisplayApvtsSync::applyEnvelopeDisplayValue(
    TSS::EnvelopeDisplay& display,
    int paramIndex,
    int value)
{
    switch (paramIndex)
    {
        case 0: display.setDelay(value, false); break;
        case 1: display.setAttack(value, false); break;
        case 2: display.setDecay(value, false); break;
        case 3: display.setSustain(value, false); break;
        case 4: display.setRelease(value, false); break;
        default: break;
    }
}

void InteractiveDisplayApvtsSync::applyTrackPointDisplayValue(int pointIndex, int value)
{
    switch (pointIndex)
    {
        case 0: trackGenerator_.setTrackPoint1(value, false); break;
        case 1: trackGenerator_.setTrackPoint2(value, false); break;
        case 2: trackGenerator_.setTrackPoint3(value, false); break;
        case 3: trackGenerator_.setTrackPoint4(value, false); break;
        case 4: trackGenerator_.setTrackPoint5(value, false); break;
        default: break;
    }
}

void InteractiveDisplayApvtsSync::parameterChanged(const juce::String& parameterId, float newValue)
{
    juce::ignoreUnused(newValue);
    applyApvtsChange(parameterId);
}
