#include "PatchMutatorPanel.h"
#include "PatchMutatorPanelInternal.h"

#include <vector>

#include "Core/Services/PatchMutator/MutatorSessionPersistence.h"
#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HierarchicalComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Toggle.h"

using namespace PatchMutatorPanelInternal;

class PatchMutatorPanel::ActionEnabledPropertyListener : public juce::ValueTree::Listener
{
public:
    ActionEnabledPropertyListener(juce::ValueTree state, std::vector<ActionEnabledBinding> bindings)
        : state_(std::move(state))
        , bindings_(std::move(bindings))
    {
        state_.addListener(this);
        syncFromState();
    }

    ~ActionEnabledPropertyListener() override
    {
        state_.removeListener(this);
    }

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        for (const auto& binding : bindings_)
        {
            if (property.toString() != binding.propertyId || binding.button == nullptr)
                continue;

            binding.button->setEnabled(static_cast<bool>(
                treeWhosePropertyHasChanged.getProperty(property, false)));
        }
    }

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override
    {
        syncFromState();
    }

private:
    void syncFromState()
    {
        for (const auto& binding : bindings_)
        {
            if (binding.button == nullptr)
                continue;

            binding.button->setEnabled(static_cast<bool>(
                state_.getProperty(binding.propertyId, false)));
        }
    }

    juce::ValueTree state_;
    std::vector<ActionEnabledBinding> bindings_;
};

// Direct parameter listener — fires on UI edits and pushChoiceToApvts, independent of APVTS ValueTree flush.
class PatchMutatorPanel::WaveSelectParameterListener : public juce::AudioProcessorParameter::Listener
{
public:
    explicit WaveSelectParameterListener(PatchMutatorPanel& owner)
        : owner_(owner)
    {
    }

    void parameterValueChanged(int, float) override
    {
        owner_.schedulePitchControlEnabledRefresh();
    }

    void parameterGestureChanged(int, bool) override {}

private:
    PatchMutatorPanel& owner_;
};

PatchMutatorPanel::PatchMutatorPanel(TSS::ISkin& skin,
                                     const PatchMutatorPanelDimensions& dims,
                                     WidgetFactory& widgetFactory,
                                     juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory);
    setupModeLine(skin, widgetFactory);
    setupPitchLine(skin, widgetFactory);
    setupHistoryLine(skin, widgetFactory);

    if (! apvts_.state.hasProperty(MutatorState::kCompareActive))
        apvts_.state.setProperty(MutatorState::kCompareActive, false, nullptr);

    actionEnabledListener_ = std::make_unique<ActionEnabledPropertyListener>(
        apvts_.state,
        std::vector<ActionEnabledBinding>{
            { MutatorState::kMutateEnabled, mutateButton_.get() },
            { MutatorState::kRetryEnabled, retryButton_.get() },
            { MutatorState::kExportEnabled, exportButton_.get() },
            { MutatorState::kDeleteEnabled, deleteButton_.get() },
            { MutatorState::kClearEnabled, clearButton_.get() } });

    waveSelectParameterListener_ = std::make_unique<WaveSelectParameterListener>(*this);
    bindWaveSelectPitchListeners();

    apvts_.state.addListener(this);
    refreshRecipeFromApvts();
    refreshHistoryComboBox();
    refreshCompareUiState();
    refreshPitchControlEnabled();
    schedulePitchControlEnabledBootRefresh();
    registerContextualHelp();

    setSize(dims_.width, dims_.height);
}

PatchMutatorPanel::~PatchMutatorPanel()
{
    stopTimer();
    unbindWaveSelectPitchListeners();
    apvts_.state.removeListener(this);
}

juce::Rectangle<int> PatchMutatorPanel::getCompareButtonBounds() const
{
    if (compareButton_ == nullptr)
        return {};

    return compareButton_->getBounds();
}

void PatchMutatorPanel::valueTreePropertyChanged(juce::ValueTree&,
                                                 const juce::Identifier& property)
{
    if (recipeHydrating_)
        return;

    const auto name = property.toString();

    if (isRecipeProperty(name))
        refreshRecipeFromApvts();

    if (isHistoryComboRefreshProperty(name))
        scheduleHistoryComboBoxRefresh();

    if (isCompareUiRefreshProperty(name))
        refreshCompareUiState();

    if (name == MutatorWidgets::kEnableDco1 || name == MutatorWidgets::kEnableDco2)
        refreshPitchControlEnabled();
}

void PatchMutatorPanel::valueTreeRedirected(juce::ValueTree&)
{
    bindWaveSelectPitchListeners();
    refreshRecipeFromApvts();
    refreshHistoryComboBox();
    refreshCompareUiState();
    refreshPitchControlEnabled();
    // Session restore / strip can settle Wave Select after this redirect — re-check briefly.
    schedulePitchControlEnabledBootRefresh();
}

void PatchMutatorPanel::schedulePitchControlEnabledRefresh()
{
    juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<PatchMutatorPanel>(this)]
    {
        if (safeThis != nullptr)
            safeThis->refreshPitchControlEnabled();
    });
}

void PatchMutatorPanel::schedulePitchControlEnabledBootRefresh()
{
    // Filet: host restore and MIDI-port strip can change Wave Select after the first paint
    // without a reliable listener tick. Rebind + refresh on the next turns.
    const auto schedulePass = [safeThis = juce::Component::SafePointer<PatchMutatorPanel>(this)](int delayMs)
    {
        const auto run = [safeThis]
        {
            if (safeThis == nullptr)
                return;

            safeThis->bindWaveSelectPitchListeners();
            safeThis->refreshPitchControlEnabled();
        };

        if (delayMs <= 0)
            juce::MessageManager::callAsync(run);
        else
            juce::Timer::callAfterDelay(delayMs, run);
    };

    schedulePass(0);
    schedulePass(50);
    schedulePass(250);
}

void PatchMutatorPanel::unbindWaveSelectPitchListeners()
{
    namespace Dco1 = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets;
    namespace Dco2 = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets;

    if (waveSelectParameterListener_ == nullptr)
        return;

    if (auto* parameter = apvts_.getParameter(Dco1::kWaveSelect))
        parameter->removeListener(waveSelectParameterListener_.get());

    if (auto* parameter = apvts_.getParameter(Dco2::kWaveSelect))
        parameter->removeListener(waveSelectParameterListener_.get());
}

void PatchMutatorPanel::bindWaveSelectPitchListeners()
{
    namespace Dco1 = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets;
    namespace Dco2 = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets;

    unbindWaveSelectPitchListeners();

    if (waveSelectParameterListener_ == nullptr)
        return;

    if (auto* parameter = apvts_.getParameter(Dco1::kWaveSelect))
        parameter->addListener(waveSelectParameterListener_.get());

    if (auto* parameter = apvts_.getParameter(Dco2::kWaveSelect))
        parameter->addListener(waveSelectParameterListener_.get());
}

bool PatchMutatorPanel::isRecipeProperty(const juce::String& propertyName)
{
    return isRecipePropertyId(propertyName);
}

void PatchMutatorPanel::refreshRecipeFromApvts()
{
    auto& state = apvts_.state;

    recipeHydrating_ = true;
    Core::MutatorSessionPersistence::initializeModeAndPitchState(state);
    hydrateModeFromApvts(state);
    hydratePitchFromApvts(state);
    hydrateRecipeTogglesFromApvts(state);
    recipeHydrating_ = false;
    refreshPitchControlEnabled();
}

void PatchMutatorPanel::hydrateModeFromApvts(const juce::ValueTree& state)
{
    if (modeComboBox_ == nullptr)
        return;

    const int modeIndex = juce::jlimit(0,
                                       Core::kMutationModeCount - 1,
                                       static_cast<int>(state.getProperty(MutatorWidgets::kMode)));
    modeComboBox_->setSelectedItemIndex(modeIndex, juce::dontSendNotification);
}

void PatchMutatorPanel::hydratePitchFromApvts(const juce::ValueTree& state)
{
    if (pitchComboBox_ == nullptr)
        return;

    const int pitchIndex = juce::jlimit(0,
                                        Core::kMutationPitchModeCount - 1,
                                        static_cast<int>(state.getProperty(MutatorWidgets::kPitch)));
    const int octaves = juce::jlimit(Core::MutationCalibration::kMinPitchOctaves,
                                     Core::MutationCalibration::kMaxPitchOctaves,
                                     static_cast<int>(state.getProperty(MutatorWidgets::kPitchOctaves)));

    const int primaryId = pitchPrimaryIdForMode(pitchIndex);
    const bool hasOctaveWindow =
        Core::pitchModeUsesOctaveWindow(Core::mutationPitchModeFromIndex(pitchIndex));

    pitchComboBox_->setSelectedIds(primaryId,
                                   hasOctaveWindow ? pitchChildIdFor(primaryId, octaves) : 0,
                                   juce::dontSendNotification);
}

void PatchMutatorPanel::handleModeComboSelectionChange()
{
    if (recipeHydrating_ || modeComboBox_ == nullptr)
        return;

    const int modeIndex = modeComboBox_->getSelectedItemIndex();
    if (modeIndex < 0)
        return;

    apvts_.state.setProperty(MutatorWidgets::kMode,
                             juce::jlimit(0, Core::kMutationModeCount - 1, modeIndex),
                             nullptr);
}

void PatchMutatorPanel::handlePitchComboSelectionChange()
{
    if (recipeHydrating_ || pitchComboBox_ == nullptr)
        return;

    const int primaryId = pitchComboBox_->getSelectedPrimaryId();
    const int pitchIndex = pitchModeIndexForPrimaryId(primaryId);

    if (pitchIndex < 0 || pitchIndex >= Core::kMutationPitchModeCount)
        return;

    recipeHydrating_ = true;

    // KEEP and FREE have no sub-menu, so the stored octave window simply stays put.
    const int childId = pitchComboBox_->getSelectedChildId();
    if (childId != 0)
    {
        apvts_.state.setProperty(MutatorWidgets::kPitchOctaves,
                                 juce::jlimit(Core::MutationCalibration::kMinPitchOctaves,
                                              Core::MutationCalibration::kMaxPitchOctaves,
                                              pitchOctavesForChildId(childId)),
                                 nullptr);
    }

    apvts_.state.setProperty(MutatorWidgets::kPitch, pitchIndex, nullptr);
    recipeHydrating_ = false;
}

void PatchMutatorPanel::hydrateRecipeTogglesFromApvts(const juce::ValueTree& state)
{
    hydrateToggleBinding(dco1Toggle_.get(), state, MutatorWidgets::kEnableDco1);
    hydrateToggleBinding(dco2Toggle_.get(), state, MutatorWidgets::kEnableDco2);
    hydrateToggleBinding(vcfVcaToggle_.get(), state, MutatorWidgets::kEnableVcfVca);
    hydrateToggleBinding(fmTrackToggle_.get(), state, MutatorWidgets::kEnableFmTrack);
    hydrateToggleBinding(rampPortamentoToggle_.get(), state, MutatorWidgets::kEnableRampPortamento);
    hydrateToggleBinding(env1Toggle_.get(), state, MutatorWidgets::kEnableEnvelope1);
    hydrateToggleBinding(env2Toggle_.get(), state, MutatorWidgets::kEnableEnvelope2);
    hydrateToggleBinding(env3Toggle_.get(), state, MutatorWidgets::kEnableEnvelope3);
    hydrateToggleBinding(lfo1Toggle_.get(), state, MutatorWidgets::kEnableLfo1);
    hydrateToggleBinding(lfo2Toggle_.get(), state, MutatorWidgets::kEnableLfo2);
    hydrateToggleBinding(enableMatrixModToggle_.get(), state, MutatorWidgets::kEnableMatrixMod);
}

void PatchMutatorPanel::timerCallback()
{
    compareBlinkVisible_ = ! compareBlinkVisible_;
    if (compareButton_ != nullptr)
    {
        // Lit = red On text; dim = normal Off text. Keep alpha at 1 so the border never greys out.
        compareButton_->setToggleState(compareBlinkVisible_, juce::dontSendNotification);
        compareButton_->setAlpha(1.0f);
    }
}

void PatchMutatorPanel::connectButtonToApvts(TSS::Button* button, const char* widgetId)
{
    if (button == nullptr)
        return;

    button->onClick = [this, widgetId]
    {
        apvts_.state.setProperty(widgetId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    };
}

void PatchMutatorPanel::connectToggleToApvts(TSS::Toggle* toggle, const char* widgetId)
{
    if (toggle == nullptr)
        return;

    toggle->onStateChange = [this, toggle, widgetId]
    {
        if (recipeHydrating_)
            return;

        apvts_.state.setProperty(widgetId, toggle->getToggleState(), nullptr);
    };
}
