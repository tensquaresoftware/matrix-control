#pragma once

#include <map>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"

namespace TSS
{
    class ISkin;
    class ModuleHeader;
    class Label;
    class Button;
    class ComboBox;
    class HierarchicalComboBox;
    class Toggle;
}

class WidgetFactory;
class FooterPanel;

class PatchMutatorPanel : public juce::Component,
                          public juce::ValueTree::Listener,
                          private juce::Timer,
                          private juce::FocusChangeListener
{
public:
    PatchMutatorPanel(TSS::ISkin& skin, const PatchMutatorPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchMutatorPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    /** Local bounds of COMPARE for MainComponent lock-film hole geometry; empty if absent. */
    juce::Rectangle<int> getCompareButtonBounds() const;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}
    void valueTreeRedirected(juce::ValueTree&) override;

    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:
    class ActionEnabledPropertyListener;
    class WaveSelectParameterListener;

    struct RecipeRowLayoutArgs
    {
        int x = 0;
        int y = 0;
        TSS::Label* label = nullptr;
        juce::Component* control = nullptr;
        TSS::Button* button = nullptr;
        TSS::Toggle* const* toggles = nullptr;
        int toggleCount = 0;
        int actionButtonWidth = 0;
    };

    PatchMutatorPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<TSS::ModuleHeader> moduleHeader_;

    std::unique_ptr<TSS::Label> modeLabel_;
    std::unique_ptr<TSS::ComboBox> modeComboBox_;
    std::unique_ptr<TSS::Button> mutateButton_;
    std::unique_ptr<TSS::Toggle> dco1Toggle_;
    std::unique_ptr<TSS::Toggle> dco2Toggle_;
    std::unique_ptr<TSS::Toggle> vcfVcaToggle_;
    std::unique_ptr<TSS::Toggle> fmTrackToggle_;
    std::unique_ptr<TSS::Toggle> rampPortamentoToggle_;

    std::unique_ptr<TSS::Label> pitchLabel_;
    std::unique_ptr<TSS::HierarchicalComboBox> pitchComboBox_;
    std::unique_ptr<TSS::Button> retryButton_;
    std::unique_ptr<TSS::Toggle> env1Toggle_;
    std::unique_ptr<TSS::Toggle> env2Toggle_;
    std::unique_ptr<TSS::Toggle> env3Toggle_;
    std::unique_ptr<TSS::Toggle> lfo1Toggle_;
    std::unique_ptr<TSS::Toggle> lfo2Toggle_;

    std::unique_ptr<TSS::Label> historyLabel_;
    std::unique_ptr<TSS::HierarchicalComboBox> historyComboBox_;
    std::unique_ptr<TSS::Button> historyPreviousButton_;
    std::unique_ptr<TSS::Button> historyNextButton_;
    std::unique_ptr<TSS::Button> compareButton_;
    std::unique_ptr<TSS::Button> deleteButton_;
    std::unique_ptr<TSS::Button> clearButton_;
    std::unique_ptr<TSS::Button> exportButton_;
    std::unique_ptr<TSS::Toggle> enableMatrixModToggle_;

    std::unique_ptr<ActionEnabledPropertyListener> actionEnabledListener_;
    std::unique_ptr<WaveSelectParameterListener> waveSelectParameterListener_;

    juce::Array<int> mutateRootIndices_;
    juce::Array<int> retryIndices_;
    std::map<int, juce::StringArray> retryLabelsByRootIndex_;

    bool recipeHydrating_ = false;
    bool historySelectionHydrating_ = false;
    bool deferHistoryComboRefresh_ = false;
    bool historyComboRefreshScheduled_ = false;
    bool historyInitialRowPresent_ = false;
    bool compareBlinkVisible_ = true;
    int contextualHelpClearGeneration_ = 0;
    std::map<juce::Component*, const char*> contextualHelpByControl_;

    // History / recipe / compare (PatchMutatorPanelHistory.cpp).
    void scheduleHistoryComboBoxRefresh();
    void rebuildRetryLabelsCacheFromApvts();
    void pruneRetryLabelsCache();
    static std::map<int, juce::StringArray> parseRetryListsByRoot(const juce::String& encoded);
    static int countFlatHistoryEntries(const juce::ValueTree& state);
    void refreshHistoryComboBox();
    void refreshCompareUiState();
    void applyCompareControlLock(bool compareActive);
    void applyCompareBlinkState(bool compareActive);
    void refreshPitchControlEnabled();
    void schedulePitchControlEnabledRefresh();
    void schedulePitchControlEnabledBootRefresh();
    void bindWaveSelectPitchListeners();
    void unbindWaveSelectPitchListeners();
    bool hasMutableAudibleDco() const;
    void handleHistoryComboSelectionChange();
    void applyHistoryRootSelectionChange(int newRootIndex, int childId);
    void syncHistorySelectionFromApvts();
    int resolveHistoryChildIdForRetry(int selectedRetryIndex) const;
    void addRetryChildrenForPrimary(int primaryId, const juce::StringArray& retryLabelList);
    static juce::StringArray parsePipeSeparatedList(const juce::String& encodedList);

    // Setup (PatchMutatorPanelSetup.cpp).
    void setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupModeLine(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupPitchLine(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void setupHistoryLine(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    void wireHistoryComboBox(TSS::ISkin& skin);
    void setupHistoryActionButtons(TSS::ISkin& skin, WidgetFactory& widgetFactory);
    std::unique_ptr<TSS::Toggle> makeRecipeToggle(TSS::ISkin& skin,
                                                  const char* displayName,
                                                  const char* widgetId);
    void connectButtonToApvts(TSS::Button* button, const char* widgetId);
    void connectToggleToApvts(TSS::Toggle* toggle, const char* widgetId);

    // Contextual help (PatchMutatorPanelContextualHelp.cpp).
    void registerContextualHelpBindings();
    void unregisterContextualHelpBindings();
    void showContextualHelpFor(juce::Component* control);
    void scheduleContextualHelpClear();
    void applyContextualHelpClearIfIdle(int generation);
    const char* helpTextForControl(juce::Component* control) const;
    juce::Component* resolveActiveContextualHelpControl() const;
    FooterPanel* resolveFooterPanel() const;
    void globalFocusChanged(juce::Component* focusedComponent) override;

    // Layout / skin (PatchMutatorPanelLayout.cpp).
    void propagateSkinsToControlWidgets(TSS::ISkin& skin);
    void propagateSkinsToToggleWidgets(TSS::ISkin& skin);
    void applyUiScaleToWidgets(float sf);
    void layoutRecipeRow(const RecipeRowLayoutArgs& args);
    void layoutHistoryLine(int x, int y);

    // Recipe hydrate (PatchMutatorPanel.cpp).
    void refreshRecipeFromApvts();
    void hydrateRecipeTogglesFromApvts(const juce::ValueTree& state);
    void hydrateModeFromApvts(const juce::ValueTree& state);
    void hydratePitchFromApvts(const juce::ValueTree& state);
    void handleModeComboSelectionChange();
    void handlePitchComboSelectionChange();
    static bool isRecipeProperty(const juce::String& propertyName);
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchMutatorPanel)
};
