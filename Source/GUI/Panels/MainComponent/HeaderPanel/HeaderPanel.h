#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Logo.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Helpers/ContextualHelpBinder.h"

namespace TSS
{
    class ISkin;
}

class HeaderPanel : public juce::Component
{
public:
    HeaderPanel(TSS::ISkin& skin, const HeaderPanelDimensions& dimensions);
    ~HeaderPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPlugin);

    void setCurrentSkinItemId(int skinItemId) { currentSkinItemId_ = skinItemId; }
    void setCurrentUiScaleId(int scaleId) { currentUiScaleId_ = scaleId; }

    std::function<void(int skinItemId)> onSkinSelected;
    std::function<void(int scaleId)> onUiScaleSelected;
    std::function<void()> onUiScaleReset;
    std::function<void()> onSettingsRequested;
    std::function<void()> onAboutRequested;
    std::function<void()> onPanicRequested;
#if JUCE_DEBUG
    std::function<void()> onUiTestsToggleRequested;
#endif

    void setPanicQueuePressureAlert(bool active);
    void setPanicMidiOutputAvailable(bool available);
    void syncEditorialUndoRedoAvailability(bool canUndo, bool canRedo);

    TSS::Led& getInstrumentActivityLed() { return instrumentActivityLed_; }
    TSS::Led& getEditorActivityLed() { return editorActivityLed_; }
    TSS::Led& getMidiToActivityLed() { return midiToActivityLed_; }
    TSS::Slider& getInputGainSlider() { return inputGainSlider_; }
    TSS::PeakIndicator& getPeakIndicator() { return peakIndicator_; }
    TSS::Button& getUndoButton() { return undoButton_; }
    TSS::Button& getRedoButton() { return redoButton_; }
    TSS::Button& getPanicButton() { return panicButton_; }

private:
    void showLogoPopup();
    void wireLogoCallbacks();
    void wireActionButtons();
    void addChildControls(TSS::ISkin& skin);
    void applyPanicButtonLook();
    void registerContextualHelp();
    void paintMidiCartouche(juce::Graphics& g);
    void paintAudioCartouche(juce::Graphics& g);
    void paintEditCartouche(juce::Graphics& g);
    void updateAudioControlsVisibility();
    void layoutLogo();
    void layoutCartouches();
    void layoutCartoucheBadgeHitAreas();

    HeaderPanelDimensions dimensions_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;
    bool panicAlertActive_ = false;
    int currentSkinItemId_ = static_cast<int>(TSS::Skin::SkinComboBoxItemId::kBlack);
    int currentUiScaleId_ = PluginIDs::Settings::ScaleLevels::kDefault;
    juce::Rectangle<int> midiCartoucheBadgeBounds_;
    juce::Rectangle<int> midiCartoucheFrameBounds_;
    int midiCartoucheStrokePx_ = 1;
    juce::Rectangle<int> audioCartoucheBadgeBounds_;
    juce::Rectangle<int> audioCartoucheFrameBounds_;
    int audioCartoucheStrokePx_ = 1;
    juce::Rectangle<int> editCartoucheBadgeBounds_;
    juce::Rectangle<int> editCartoucheFrameBounds_;
    int editCartoucheStrokePx_ = 1;

    TSS::Logo logo_;
    TSS::Led instrumentActivityLed_;
    TSS::Label keyboardFromLabel_;
    TSS::Led editorActivityLed_;
    TSS::Label midiFromLabel_;
    TSS::Led midiToActivityLed_;
    TSS::Label midiToLabel_;
    TSS::Label inputGainLabel_;
    TSS::Slider inputGainSlider_;
    TSS::PeakIndicator peakIndicator_;
    TSS::Button undoButton_;
    TSS::Button redoButton_;
    TSS::Button panicButton_;
    juce::Component editCartoucheBadgeHitArea_;
    juce::Component midiCartoucheBadgeHitArea_;
    juce::Component audioCartoucheBadgeHitArea_;

    std::unique_ptr<TSS::ContextualHelpBinder> contextualHelpBinder_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};
