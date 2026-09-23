// Extracted from PatchMutatorPanel.cpp for modular maintenance.
// History combo, compare lock / blink, and recipe-adjacent list parsing.

#include "PatchMutatorPanel.h"
#include "PatchMutatorPanelInternal.h"

#include "Core/Services/PatchMutator/MutationNaming.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HierarchicalComboBox.h"
#include "GUI/Widgets/Label.h"

using namespace PatchMutatorPanelInternal;

juce::StringArray PatchMutatorPanel::parsePipeSeparatedList(const juce::String& encodedList)
{
    if (encodedList.isEmpty())
        return {};

    return juce::StringArray::fromTokens(encodedList, "|", "");
}

void PatchMutatorPanel::scheduleHistoryComboBoxRefresh()
{
    if (deferHistoryComboRefresh_ || historyComboRefreshScheduled_)
        return;

    historyComboRefreshScheduled_ = true;

    juce::Component::SafePointer<PatchMutatorPanel> safePanel(this);
    juce::MessageManager::callAsync([safePanel]
    {
        if (safePanel == nullptr)
            return;

        safePanel->historyComboRefreshScheduled_ = false;
        safePanel->refreshHistoryComboBox();
    });
}

std::map<int, juce::StringArray> PatchMutatorPanel::parseRetryListsByRoot(const juce::String& encoded)
{
    std::map<int, juce::StringArray> result;
    if (encoded.isEmpty())
        return result;

    for (const auto& part : juce::StringArray::fromTokens(encoded, ";", ""))
    {
        const int eq = part.indexOfChar('=');
        if (eq <= 0)
            continue;

        const int rootIndex = part.substring(0, eq).getIntValue();
        result[rootIndex] = parsePipeSeparatedList(part.substring(eq + 1));
    }

    return result;
}

int PatchMutatorPanel::countFlatHistoryEntries(const juce::ValueTree& state)
{
    // INITIAL is a navigable slot too (engine flat list leads with it).
    const int initialSlotCount = isHistoryInitialRowAvailable(state) ? 1 : 0;

    const auto byRoot = parseRetryListsByRoot(state.getProperty(MutatorState::kHistoryRetryListsByRoot).toString());
    if (! byRoot.empty())
    {
        int count = initialSlotCount;
        for (const auto& entry : byRoot)
            count += entry.second.size();
        return count;
    }

    return initialSlotCount
           + parsePipeSeparatedList(state.getProperty(MutatorState::kHistoryMutateList).toString()).size();
}

void PatchMutatorPanel::rebuildRetryLabelsCacheFromApvts()
{
    retryLabelsByRootIndex_ = parseRetryListsByRoot(
        apvts_.state.getProperty(MutatorState::kHistoryRetryListsByRoot).toString());

    // Fallback for older in-session state that only has the selected-root mirror.
    if (retryLabelsByRootIndex_.empty())
    {
        const int selectedMutateRootIndex = static_cast<int>(
            apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
        const auto retryLabelList = parsePipeSeparatedList(
            apvts_.state.getProperty(MutatorState::kHistoryRetryList).toString());
        if (selectedMutateRootIndex >= 0 && ! retryLabelList.isEmpty())
            retryLabelsByRootIndex_[selectedMutateRootIndex] = retryLabelList;
    }
}

void PatchMutatorPanel::pruneRetryLabelsCache()
{
    for (auto it = retryLabelsByRootIndex_.begin(); it != retryLabelsByRootIndex_.end();)
    {
        if (! mutateRootIndices_.contains(it->first))
            it = retryLabelsByRootIndex_.erase(it);
        else
            ++it;
    }
}

void PatchMutatorPanel::addRetryChildrenForPrimary(int primaryId, const juce::StringArray& retryLabelList)
{
    if (primaryId <= 0 || primaryId > mutateRootIndices_.size())
        return;

    const int rootIndex = mutateRootIndices_[primaryId - 1];
    const auto display = Core::MutationNaming::buildHistorySubmenuDisplay(rootIndex, retryLabelList);
    if (display.labels.isEmpty())
        return;

    jassert(display.labels.size() == display.retryIndices.size());

    const bool trackSelectionIndices = (rootIndex
                                        == static_cast<int>(apvts_.state.getProperty(
                                            MutatorState::kSelectedMutateRootIndex, -1)));

    if (trackSelectionIndices)
        retryIndices_.clear();

    for (int i = 0; i < display.labels.size(); ++i)
    {
        const int childId = i + 1;
        if (trackSelectionIndices)
            retryIndices_.add(display.retryIndices[i]);
        historyComboBox_->addChildItem(primaryId, childId, display.labels[i]);
    }
}

void PatchMutatorPanel::applyHistoryRootSelectionChange(int newRootIndex, int childId)
{
    // Hierarchical UX: honour the submenu child clicked while changing M;
    // fall back to root-only when no child is selected.
    int newRetryIndex = MutatorState::kSelectedRetryRootOnly;
    const auto cacheIt = retryLabelsByRootIndex_.find(newRootIndex);
    if (childId > 0 && cacheIt != retryLabelsByRootIndex_.end())
    {
        const auto display = Core::MutationNaming::buildHistorySubmenuDisplay(
            newRootIndex, cacheIt->second);
        if (childId <= display.retryIndices.size())
            newRetryIndex = display.retryIndices[childId - 1];
    }

    deferHistoryComboRefresh_ = true;
    apvts_.state.setProperty(MutatorState::kSelectedRetryIndex, newRetryIndex, nullptr);
    apvts_.state.setProperty(MutatorState::kSelectedMutateRootIndex, newRootIndex, nullptr);
    deferHistoryComboRefresh_ = false;
    refreshHistoryComboBox();
}

void PatchMutatorPanel::handleHistoryComboSelectionChange()
{
    if (historySelectionHydrating_ || historyComboBox_ == nullptr)
        return;

    const int primaryId = historyComboBox_->getSelectedPrimaryId();

    if (primaryId == kHistoryInitialPrimaryId)
    {
        // Keep M / R untouched: they are where leaving INITIAL comes back to.
        apvts_.state.setProperty(MutatorState::kInitialSelected, true, nullptr);
        return;
    }

    if (primaryId <= 0 || primaryId > mutateRootIndices_.size())
        return;

    if (isHistoryInitialSelected(apvts_.state))
        apvts_.state.setProperty(MutatorState::kInitialSelected, false, nullptr);

    const int newRootIndex = mutateRootIndices_[primaryId - 1];
    const int currentRootIndex = static_cast<int>(
        apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
    const int childId = historyComboBox_->getSelectedChildId();

    if (newRootIndex != currentRootIndex)
    {
        applyHistoryRootSelectionChange(newRootIndex, childId);
        return;
    }

    if (childId <= 0)
    {
        apvts_.state.setProperty(MutatorState::kSelectedRetryIndex,
                                 MutatorState::kSelectedRetryRootOnly,
                                 nullptr);
        return;
    }

    if (childId > retryIndices_.size())
        return;

    apvts_.state.setProperty(MutatorState::kSelectedRetryIndex,
                             retryIndices_[childId - 1],
                             nullptr);
}

void PatchMutatorPanel::refreshHistoryComboBox()
{
    if (historyComboBox_ == nullptr)
        return;

    const auto mutateLabelList = parsePipeSeparatedList(
        apvts_.state.getProperty(MutatorState::kHistoryMutateList).toString());
    historySelectionHydrating_ = true;
    historyComboBox_->clear();
    mutateRootIndices_.clear();
    retryIndices_.clear();

    if (mutateLabelList.isEmpty())
    {
        retryLabelsByRootIndex_.clear();
        historyInitialRowPresent_ = false;
        historyComboBox_->setTextWhenNothingSelected(MutatorDisplayNames::kEmptyHistorySentinel);
        historyComboBox_->setSelectedIds(0, 0, juce::dontSendNotification);
        historySelectionHydrating_ = false;
        refreshCompareUiState();
        return;
    }

    rebuildRetryLabelsCacheFromApvts();

    historyInitialRowPresent_ = isHistoryInitialRowAvailable(apvts_.state);
    if (historyInitialRowPresent_)
        historyComboBox_->addPrimaryItem(kHistoryInitialPrimaryId, kHistoryInitialLabel);

    for (int i = 0; i < mutateLabelList.size(); ++i)
    {
        const auto label = mutateLabelList[i];
        const int primaryId = i + 1;
        const int rootIndex = label.substring(1, 3).getIntValue();
        mutateRootIndices_.add(rootIndex);
        historyComboBox_->addPrimaryItem(primaryId, label);
    }

    pruneRetryLabelsCache();

    for (int i = 0; i < mutateRootIndices_.size(); ++i)
    {
        const int rootIndex = mutateRootIndices_[i];
        const auto cacheIt = retryLabelsByRootIndex_.find(rootIndex);
        if (cacheIt == retryLabelsByRootIndex_.end() || cacheIt->second.isEmpty())
            continue;

        addRetryChildrenForPrimary(i + 1, cacheIt->second);
    }

    historyComboBox_->setEnabled(true);
    syncHistorySelectionFromApvts();
    historySelectionHydrating_ = false;
    refreshCompareUiState();
}

void PatchMutatorPanel::syncHistorySelectionFromApvts()
{
    if (historyComboBox_ == nullptr)
        return;

    // Compare auditions the origin, so it shows the same row as a manual INITIAL pick.
    const bool compareActive = static_cast<bool>(
        apvts_.state.getProperty(MutatorState::kCompareActive, false));
    if (historyInitialRowPresent_ && (compareActive || isHistoryInitialSelected(apvts_.state)))
    {
        historyComboBox_->setSelectedIds(kHistoryInitialPrimaryId, 0, juce::dontSendNotification);
        return;
    }

    const int selectedMutateRootIndex = static_cast<int>(
        apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
    if (selectedMutateRootIndex < 0)
    {
        historyComboBox_->setSelectedIds(0, 0, juce::dontSendNotification);
        return;
    }

    const int primaryId = mutateRootIndices_.indexOf(selectedMutateRootIndex) + 1;
    if (primaryId <= 0)
    {
        historyComboBox_->setSelectedIds(0, 0, juce::dontSendNotification);
        return;
    }

    const int selectedRetryIndex = static_cast<int>(apvts_.state.getProperty(
        MutatorState::kSelectedRetryIndex, MutatorState::kSelectedRetryRootOnly));

    historyComboBox_->setSelectedIds(primaryId,
                                     resolveHistoryChildIdForRetry(selectedRetryIndex),
                                     juce::dontSendNotification);
}

int PatchMutatorPanel::resolveHistoryChildIdForRetry(int selectedRetryIndex) const
{
    const int childId = retryIndices_.indexOf(selectedRetryIndex) + 1;
    if (childId > 0)
        return childId;

    // Root-only, or orphan retry not present in this root's list — select N2 root recall.
    return retryIndices_.isEmpty() ? 0 : 1;
}

void PatchMutatorPanel::applyCompareBlinkState(bool compareActive)
{
    if (compareActive)
    {
        compareBlinkVisible_ = true;
        if (compareButton_ != nullptr)
            compareButton_->setToggleState(true, juce::dontSendNotification);
        startTimerHz(2);
        return;
    }

    stopTimer();
    if (compareButton_ == nullptr)
        return;

    compareButton_->setToggleState(false, juce::dontSendNotification);
    compareButton_->setAlpha(1.0f);
}

bool PatchMutatorPanel::hasMutableAudibleDco() const
{
    namespace Dco1 = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets;
    namespace Dco2 = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets;

    const auto& state = apvts_.state;
    const bool dco1Mutable = static_cast<bool>(state.getProperty(MutatorWidgets::kEnableDco1, false));
    const bool dco2Mutable = static_cast<bool>(state.getProperty(MutatorWidgets::kEnableDco2, false));

    return (dco1Mutable && isWaveSelectAudible(apvts_, Dco1::kWaveSelect))
           || (dco2Mutable && isWaveSelectAudible(apvts_, Dco2::kWaveSelect));
}

void PatchMutatorPanel::refreshPitchControlEnabled()
{
    if (pitchComboBox_ == nullptr)
        return;

    const bool compareActive = static_cast<bool>(
        apvts_.state.getProperty(MutatorState::kCompareActive, false));
    const bool enabled = ! compareActive && hasMutableAudibleDco();

    pitchComboBox_->setEnabled(enabled);
    pitchComboBox_->setInactiveAppearance(! enabled);
    if (pitchLabel_ != nullptr)
        pitchLabel_->setEnabled(enabled);
}

void PatchMutatorPanel::refreshCompareUiState()
{
    const bool compareActive = static_cast<bool>(
        apvts_.state.getProperty(MutatorState::kCompareActive, false));
    const auto mutateLabelList = parsePipeSeparatedList(
        apvts_.state.getProperty(MutatorState::kHistoryMutateList).toString());
    const int selectedMutateRootIndex = static_cast<int>(
        apvts_.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
    const bool historyEmpty = mutateLabelList.isEmpty() || selectedMutateRootIndex < 0;
    const int flatHistoryEntryCount = countFlatHistoryEntries(apvts_.state);
    // Manual INITIAL already shows the origin — COMPARE has nothing to swap in.
    const bool initialSelectedWithoutCompare = ! compareActive && isHistoryInitialSelected(apvts_.state);

    applyCompareControlLock(compareActive);

    if (compareButton_ != nullptr)
    {
        compareButton_->setEnabled(compareActive || (! historyEmpty && ! initialSelectedWithoutCompare));
        compareButton_->setAlpha(1.0f);
        if (! compareActive)
            compareButton_->setToggleState(false, juce::dontSendNotification);
    }

    if (historyComboBox_ != nullptr)
        historyComboBox_->setEnabled(! compareActive && ! historyEmpty);

    // Nav is useful only when circular step can change selection (≥2 flat entries).
    const bool historyNavEnabled = ! compareActive && flatHistoryEntryCount >= 2;
    if (historyPreviousButton_ != nullptr)
        historyPreviousButton_->setEnabled(historyNavEnabled);
    if (historyNextButton_ != nullptr)
        historyNextButton_->setEnabled(historyNavEnabled);

    applyCompareBlinkState(compareActive);
}

void PatchMutatorPanel::applyCompareControlLock(bool compareActive)
{
    const auto lockControl = [compareActive](juce::Component* control)
    {
        if (control == nullptr)
            return;

        control->setEnabled(! compareActive);
    };

    lockControl(modeComboBox_.get());
    refreshPitchControlEnabled();
    lockControl(dco1Toggle_.get());
    lockControl(dco2Toggle_.get());
    lockControl(vcfVcaToggle_.get());
    lockControl(fmTrackToggle_.get());
    lockControl(rampPortamentoToggle_.get());
    lockControl(env1Toggle_.get());
    lockControl(env2Toggle_.get());
    lockControl(env3Toggle_.get());
    lockControl(lfo1Toggle_.get());
    lockControl(lfo2Toggle_.get());
    lockControl(enableMatrixModToggle_.get());

    // Action buttons: locked -> disabled skin paint; unlocked -> restore from their enabled mirror.
    const auto lockActionButton = [this, compareActive](TSS::Button* button, const char* mirrorId)
    {
        if (button == nullptr)
            return;

        const bool enabled = compareActive
                                 ? false
                                 : static_cast<bool>(apvts_.state.getProperty(mirrorId, false));
        button->setEnabled(enabled);
    };

    lockActionButton(mutateButton_.get(), MutatorState::kMutateEnabled);
    lockActionButton(retryButton_.get(), MutatorState::kRetryEnabled);
    lockActionButton(deleteButton_.get(), MutatorState::kDeleteEnabled);
    lockActionButton(clearButton_.get(), MutatorState::kClearEnabled);
    lockActionButton(exportButton_.get(), MutatorState::kExportEnabled);

    refreshMutateRetryHoverLabels();

    if (historyComboBox_ != nullptr)
        historyComboBox_->setEnabled(! compareActive);

    // History prev/next enablement is owned by refreshCompareUiState (needs flat entry count).
}
