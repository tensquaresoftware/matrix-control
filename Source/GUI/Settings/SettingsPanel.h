#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"

namespace TSS
{
    class ISkin;
}

class SettingsPanel : public juce::Component
{
public:
    // Content = label column 120 + control column 140 (no gap); outer = content + padding 16*2.
    // Control column fits UTILITY (LOAD | SAVE AS | INIT) at 44+4+44+4+44.
    static constexpr int kDesignWidth = 292;
    static constexpr int kDesignHeight = 456;

    SettingsPanel(TSS::ISkin& skin, bool isPluginMode);
    ~SettingsPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void setPluginMode(bool isPluginMode);

    TSS::Slider& getHardwareLatencySlider() { return *hardwareLatencySlider_; }
    TSS::ComboBox& getMatrix1000PatchesCombo() { return *matrix1000PatchesCombo_; }
    TSS::ComboBox& getComputerPatchesCombo() { return *computerPatchesCombo_; }
    TSS::ComboBox& getUnsavedStateCombo() { return *unsavedStateCombo_; }
    TSS::ComboBox& getDeleteWarningCombo() { return *deleteWarningCombo_; }

    TSS::Button& getPatchSaveAsInitButton() { return *patchSaveAsInitButton_; }
    TSS::Button& getPatchDeleteInitButton() { return *patchDeleteInitButton_; }
    TSS::Button& getMasterLoadButton() { return *masterLoadButton_; }
    TSS::Button& getMasterSaveAsButton() { return *masterSaveAsButton_; }
    TSS::Button& getMasterInitButton() { return *masterInitButton_; }
    TSS::Button& getMasterSaveAsInitButton() { return *masterSaveAsInitButton_; }
    TSS::Button& getMasterDeleteInitButton() { return *masterDeleteInitButton_; }

    void refreshInitTemplateDeleteEnablement(bool patchInitExists, bool masterInitExists);

private:
    struct SectionHeaderLayoutArgs
    {
        TSS::Label* title = nullptr;
        TSS::HorizontalSeparator* separator = nullptr;
        int controlHeight = 0;
        int separatorHeight = 0;
        int rowGap = 0;
    };

    struct RowLayoutMetrics
    {
        int rowGap = 0;
        int labelWidth = 0;
        int sliderWidth = 0;
        int controlHeight = 0;
        int separatorHeight = 0;
        int comboWidth = 0;
        int buttonGap = 0;
        int utilityLoadWidth = 0;
        int utilitySaveAsWidth = 0;
        int utilityInitWidth = 0;
        int saveAsInitWidth = 0;
        int deleteInitWidth = 0;
    };

    struct LabeledControlRowArgs
    {
        TSS::Label* label = nullptr;
        juce::Component* control = nullptr;
        int controlWidth = 0;
    };

    void setupPatchSection(TSS::ISkin& skin);
    void setupPatchMutatorSection(TSS::ISkin& skin);
    void setupMasterSection(TSS::ISkin& skin);
    void populateComboItems();
    void applyComboPopupLooks(TSS::ISkin& skin);
    void applyChildLooks(TSS::ISkin& skin);

    std::unique_ptr<TSS::Label> makeLabel(TSS::ISkin& skin, int width, const juce::String& text);
    std::unique_ptr<TSS::ComboBox> makeCombo(TSS::ISkin& skin, int width);
    std::unique_ptr<TSS::Button> makeButton(TSS::ISkin& skin, int width, const juce::String& text);
    std::unique_ptr<TSS::HorizontalSeparator> makeSeparator(TSS::ISkin& skin);

    void updateModeSpecificVisibility();
    void layoutContent(juce::Rectangle<int> bounds);
    void layoutPatchSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutPatchMutatorSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutMasterSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics);
    void layoutSectionHeader(juce::Rectangle<int>& bounds, const SectionHeaderLayoutArgs& args);
    void layoutLabeledControlRow(juce::Rectangle<int>& bounds,
                                 const RowLayoutMetrics& metrics,
                                 const LabeledControlRowArgs& args);
    void layoutPlaceholderRow(juce::Rectangle<int>& bounds,
                              const RowLayoutMetrics& metrics,
                              TSS::Label& label,
                              TSS::Label& placeholder);
    struct ButtonRowLayoutArgs
    {
        TSS::Label* label = nullptr;
        std::initializer_list<TSS::Button*> buttons;
        std::initializer_list<int> buttonWidths;
    };

    void layoutButtonRow(juce::Rectangle<int>& bounds,
                         const RowLayoutMetrics& metrics,
                         const ButtonRowLayoutArgs& args);

    inline constexpr static int kPadding_ = 16;
    inline constexpr static int kRowGap_ = 12;
    // Design gap from section title row to the separator stroke (not the separator component box).
    inline constexpr static int kSectionTitleGap_ = 4;
    inline constexpr static int kControlHeight_ = 20;
    // Match HorizontalSeparator line thickness so the stroke sits at the top of the gap+line stack.
    inline constexpr static int kSeparatorHeight_ = 1;
    inline constexpr static int kLabelWidth_ = 120;
    inline constexpr static int kComboWidth_ = 140;
    inline constexpr static int kSliderWidth_ = 72;
    inline constexpr static int kButtonGap_ = 4;
    inline constexpr static int kUtilityLoadWidth_ = 44;
    inline constexpr static int kUtilitySaveAsWidth_ = 44;
    inline constexpr static int kUtilityInitWidth_ = 44;
    inline constexpr static int kSaveAsInitWidth_ = 68;
    inline constexpr static int kDeleteInitWidth_ = 68;
    inline constexpr static int kContentWidth_ = kLabelWidth_ + kComboWidth_;
    static_assert(kDesignWidth == kContentWidth_ + kPadding_ * 2);
    static_assert(kUtilityLoadWidth_ + kButtonGap_ + kUtilitySaveAsWidth_ + kButtonGap_
                      + kUtilityInitWidth_
                  <= kComboWidth_);
    static_assert(kSaveAsInitWidth_ + kButtonGap_ + kDeleteInitWidth_ <= kComboWidth_);

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool isPluginMode_ = false;

    std::unique_ptr<TSS::Label> patchSectionLabel_;
    std::unique_ptr<TSS::HorizontalSeparator> patchSectionSeparator_;
    std::unique_ptr<TSS::Label> matrix1000PatchesLabel_;
    std::unique_ptr<TSS::ComboBox> matrix1000PatchesCombo_;
    std::unique_ptr<TSS::Label> computerPatchesLabel_;
    std::unique_ptr<TSS::ComboBox> computerPatchesCombo_;
    std::unique_ptr<TSS::Label> unsavedStateLabel_;
    std::unique_ptr<TSS::ComboBox> unsavedStateCombo_;
    std::unique_ptr<TSS::Label> patchInitTemplateLabel_;
    std::unique_ptr<TSS::Button> patchSaveAsInitButton_;
    std::unique_ptr<TSS::Button> patchDeleteInitButton_;

    std::unique_ptr<TSS::Label> patchMutatorSectionLabel_;
    std::unique_ptr<TSS::HorizontalSeparator> patchMutatorSectionSeparator_;
    std::unique_ptr<TSS::Label> deleteWarningLabel_;
    std::unique_ptr<TSS::ComboBox> deleteWarningCombo_;
    std::unique_ptr<TSS::Label> defragHistoryLabel_;
    std::unique_ptr<TSS::Label> defragHistoryPlaceholder_;

    std::unique_ptr<TSS::Label> masterSectionLabel_;
    std::unique_ptr<TSS::HorizontalSeparator> masterSectionSeparator_;
    std::unique_ptr<TSS::Label> hardwareLatencyLabel_;
    std::unique_ptr<TSS::Slider> hardwareLatencySlider_;
    std::unique_ptr<TSS::Label> masterUtilityLabel_;
    std::unique_ptr<TSS::Button> masterLoadButton_;
    std::unique_ptr<TSS::Button> masterSaveAsButton_;
    std::unique_ptr<TSS::Button> masterInitButton_;
    std::unique_ptr<TSS::Label> masterInitTemplateLabel_;
    std::unique_ptr<TSS::Button> masterSaveAsInitButton_;
    std::unique_ptr<TSS::Button> masterDeleteInitButton_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPanel)
};
