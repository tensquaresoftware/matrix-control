#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Settings/SettingsAudioPage.h"
#include "GUI/Settings/SettingsMidiPage.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace TSS
{
    class ISkin;
}

class SettingsPanel : public juce::Component
{
public:
    static constexpr int kDesignWidth = SettingsShellMetrics::kContentWidth;

    SettingsPanel(TSS::ISkin& skin, bool isPluginMode);
    ~SettingsPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPluginMode);
    void setDeviceType(MatrixDeviceTypes::Type deviceType);
    void setActiveTab(int tabId);
    int getActiveTab() const noexcept { return activeTabId_; }

    void attachAudioPage(SettingsAudioPage::Config config);
    SettingsAudioPage* getAudioPage() const noexcept { return audioPage_.get(); }

    void attachMidiPage(SettingsMidiPage::Config config);
    SettingsMidiPage* getMidiPage() const noexcept { return midiPage_.get(); }

    void registerContextualHelp(TSS::ContextualHelpBinder::FooterResolver resolveFooter);

    TSS::Slider& getHardwareLatencySlider() { return *hardwareLatencySlider_; }
    TSS::ComboBox& getEpromTypeCombo() { return *epromTypeCombo_; }
    TSS::ComboBox& getMatrix1000PatchesCombo() { return *matrix1000PatchesCombo_; }
    TSS::ComboBox& getComputerPatchesCombo() { return *computerPatchesCombo_; }
    TSS::ComboBox& getUiScaleCombo() { return *uiScaleCombo_; }
    TSS::ComboBox& getSkinCombo() { return *skinCombo_; }
    TSS::ComboBox& getInfoMessageCombo() { return *infoMessageCombo_; }
    TSS::ComboBox& getContextualHelpCombo() { return *contextualHelpCombo_; }
    TSS::ComboBox& getGettingStartedAutoOpenCombo() { return *gettingStartedAutoOpenCombo_; }
    TSS::Button& getRunSetupAgainButton() { return *runSetupAgainButton_; }
    TSS::ComboBox& getUnsavedStateCombo() { return *unsavedStateCombo_; }
    TSS::ComboBox& getDeleteWarningCombo() { return *deleteWarningCombo_; }

    TSS::Button& getPatchSaveAsInitButton() { return *patchSaveAsInitButton_; }
    TSS::Button& getPatchDeleteInitButton() { return *patchDeleteInitButton_; }
    TSS::Button& getDefragHistoryButton() { return *defragHistoryButton_; }
    TSS::Button& getMasterLoadButton() { return *masterLoadButton_; }
    TSS::Button& getMasterSaveAsButton() { return *masterSaveAsButton_; }
    TSS::Button& getMasterInitButton() { return *masterInitButton_; }
    TSS::Button& getMasterSaveAsInitButton() { return *masterSaveAsInitButton_; }
    TSS::Button& getMasterDeleteInitButton() { return *masterDeleteInitButton_; }

    void refreshInitTemplateDeleteEnablement(bool patchInitExists, bool masterInitExists);
    void refreshDefragHistoryEnablement(bool hasMutationHistory);

    /** Rebuild EPROM TYPE items for the current device family; returns selected id after coerce. */
    int refreshEpromTypeItems(int preferredSelectedId);

private:
    struct RowLayoutMetrics
    {
        int rowGap = 0;
        int labelWidth = 0;
        int sliderWidth = 0;
        int controlHeight = 0;
        int comboWidth = 0;
        int buttonGap = 0;
        int utilityLoadWidth = 0;
        int utilitySaveAsWidth = 0;
        int utilityInitWidth = 0;
        int saveAsInitWidth = 0;
        int deleteInitWidth = 0;
        int defragButtonWidth = 0;
    };

    struct LabeledControlRowArgs
    {
        TSS::Label* label = nullptr;
        juce::Component* control = nullptr;
        int controlWidth = 0;
    };

    void setupInterfaceSection(TSS::ISkin& skin);
    void setupDeviceSection(TSS::ISkin& skin);
    void setupPatchSection(TSS::ISkin& skin);
    void setupPatchMutatorSection(TSS::ISkin& skin);
    void setupMasterSection(TSS::ISkin& skin);
    void populateComboItems();
    void applyComboPopupLooks(TSS::ISkin& skin);
    void applyChildLooks(TSS::ISkin& skin);

    std::unique_ptr<TSS::Label> makeLabel(TSS::ISkin& skin, int width, const juce::String& text);
    std::unique_ptr<TSS::ComboBox> makeCombo(TSS::ISkin& skin, int width);
    std::unique_ptr<TSS::Button> makeButton(TSS::ISkin& skin, int width, const juce::String& text);

    void updatePageVisibility();
    void setInterfaceSectionVisible(bool visible);
    void layoutContent(juce::Rectangle<int> bounds);
    void layoutInterfaceSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutDeviceSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutMidiSection(juce::Rectangle<int>& bounds);
    void layoutAudioSection(juce::Rectangle<int>& bounds);
    void layoutPatchSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutPatchMutatorSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutMasterSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutLabeledControlRow(juce::Rectangle<int>& bounds,
                                 const RowLayoutMetrics& metrics,
                                 const LabeledControlRowArgs& args);
    struct ButtonRowLayoutArgs
    {
        TSS::Label* label = nullptr;
        std::initializer_list<TSS::Button*> buttons;
        std::initializer_list<int> buttonWidths;
    };

    void layoutButtonRow(juce::Rectangle<int>& bounds,
                         const RowLayoutMetrics& metrics,
                         const ButtonRowLayoutArgs& args);
    void layoutButtonOnlyRow(juce::Rectangle<int>& bounds,
                             const RowLayoutMetrics& metrics,
                             TSS::Button& button,
                             int buttonWidth);

    inline constexpr static int kPadding_ = SettingsShellMetrics::kPadding;
    inline constexpr static int kRowGap_ = 8;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kLabelWidth_ = SettingsShellMetrics::kLabelWidth;
    inline constexpr static int kComboWidth_ = SettingsShellMetrics::kControlColumnWidth;
    inline constexpr static int kSliderWidth_ = 72;
    inline constexpr static int kButtonGap_ = 4;
    inline constexpr static int kUtilityLoadWidth_ = 44;
    inline constexpr static int kUtilitySaveAsWidth_ = 44;
    inline constexpr static int kUtilityInitWidth_ = 44;
    inline constexpr static int kSaveAsInitWidth_ = 68;
    inline constexpr static int kDeleteInitWidth_ = 68;
    inline constexpr static int kDefragButtonWidth_ = 68;
    inline constexpr static int kContentWidth_ = kLabelWidth_ + kComboWidth_;
    static_assert(kDesignWidth == kContentWidth_ + kPadding_ * 2);
    static_assert(kUtilityLoadWidth_ + kButtonGap_ + kUtilitySaveAsWidth_ + kButtonGap_
                      + kUtilityInitWidth_
                  <= kComboWidth_);
    static_assert(kSaveAsInitWidth_ + kButtonGap_ + kDeleteInitWidth_ <= kComboWidth_);
    static_assert(kDefragButtonWidth_ <= kComboWidth_);

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;
    int activeTabId_ = 1;
    MatrixDeviceTypes::Type deviceType_ = MatrixDeviceTypes::Type::kUnknown;

    std::unique_ptr<TSS::Label> uiScaleLabel_;
    std::unique_ptr<TSS::ComboBox> uiScaleCombo_;
    std::unique_ptr<TSS::Label> skinLabel_;
    std::unique_ptr<TSS::ComboBox> skinCombo_;
    std::unique_ptr<TSS::Label> infoMessageLabel_;
    std::unique_ptr<TSS::ComboBox> infoMessageCombo_;
    std::unique_ptr<TSS::Label> contextualHelpLabel_;
    std::unique_ptr<TSS::ComboBox> contextualHelpCombo_;
    std::unique_ptr<TSS::Label> gettingStartedLabel_;
    std::unique_ptr<TSS::ComboBox> gettingStartedAutoOpenCombo_;
    std::unique_ptr<TSS::Button> runSetupAgainButton_;

    std::unique_ptr<TSS::Label> hardwareLatencyLabel_;
    std::unique_ptr<TSS::Slider> hardwareLatencySlider_;
    std::unique_ptr<TSS::Label> epromTypeLabel_;
    std::unique_ptr<TSS::ComboBox> epromTypeCombo_;

    std::unique_ptr<TSS::Label> matrix1000PatchesLabel_;
    std::unique_ptr<TSS::ComboBox> matrix1000PatchesCombo_;
    std::unique_ptr<TSS::Label> computerPatchesLabel_;
    std::unique_ptr<TSS::ComboBox> computerPatchesCombo_;
    std::unique_ptr<TSS::Label> unsavedStateLabel_;
    std::unique_ptr<TSS::ComboBox> unsavedStateCombo_;
    std::unique_ptr<TSS::Label> patchInitTemplateLabel_;
    std::unique_ptr<TSS::Button> patchSaveAsInitButton_;
    std::unique_ptr<TSS::Button> patchDeleteInitButton_;

    std::unique_ptr<TSS::Label> deleteWarningLabel_;
    std::unique_ptr<TSS::ComboBox> deleteWarningCombo_;
    std::unique_ptr<TSS::Label> defragHistoryLabel_;
    std::unique_ptr<TSS::Button> defragHistoryButton_;

    std::unique_ptr<TSS::Label> masterUtilityLabel_;
    std::unique_ptr<TSS::Button> masterLoadButton_;
    std::unique_ptr<TSS::Button> masterSaveAsButton_;
    std::unique_ptr<TSS::Button> masterInitButton_;
    std::unique_ptr<TSS::Label> masterInitTemplateLabel_;
    std::unique_ptr<TSS::Button> masterSaveAsInitButton_;
    std::unique_ptr<TSS::Button> masterDeleteInitButton_;

    std::unique_ptr<SettingsMidiPage> midiPage_;
    std::unique_ptr<SettingsAudioPage> audioPage_;
    std::unique_ptr<TSS::ContextualHelpBinder> contextualHelpBinder_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanel)
};
