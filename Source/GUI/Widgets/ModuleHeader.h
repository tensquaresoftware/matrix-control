#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Looks/WidgetLooks.h"

class WidgetFactory;

namespace tss
{
    class ISkin;
    class Button;

    class ModuleHeader : public juce::Component
    {
    public:
        enum class ColourVariant
        {
            Blue,
            Orange
        };

        enum class ButtonSet
        {
            None,
            InitOnly,
            InitCopyPaste
        };

        enum class ColumnLayout
        {
            PatchEdit,
            MasterEdit
        };

        struct WithActionsSpec
        {
            ISkin& skin;
            WidgetFactory& widgetFactory;
            juce::AudioProcessorValueTreeState& apvts;
            juce::String moduleId;
            ColumnLayout columnLayout;
            ButtonSet buttonSet;
            juce::String initWidgetId;
            juce::String copyWidgetId;
            juce::String pasteWidgetId;
        };

        explicit ModuleHeader(int width, int height, const ModuleHeaderLook& look, ColourVariant variant,
                              const juce::String& text);

        explicit ModuleHeader(const WithActionsSpec& spec);

        ~ModuleHeader() override;

        void setLook(const ModuleHeaderLook& look);
        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);

        static int getDesignHeight() noexcept;

        void setText(const juce::String& text);
        juce::String getText() const { return text_; }

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        enum class Presentation
        {
            TitleOnly,
            TitleWithActions
        };

        int getTitleBandWidthDesign() const;
        juce::Rectangle<float> getTitlePaintBounds() const;

        void createInitButton(const WithActionsSpec& spec);
        void createCopyPasteButtons(const WithActionsSpec& spec);

        void layoutInitOnlyButtons();
        void layoutInitCopyPasteButtons();
        void applyButtonLooksFromSkin(ISkin& skin);

        Presentation presentation_ = Presentation::TitleOnly;
        ColumnLayout columnLayout_ = ColumnLayout::PatchEdit;
        ButtonSet buttonSet_ = ButtonSet::None;

        inline constexpr static int kTextLeftPadding_ = 0;
        inline constexpr static int kTextAreaHeight_ = 20;
        inline constexpr static int kLineThickness_ = 4;

        ModuleHeaderLook look_{};
        int width_ = 0;
        int height_ = 0;
        int titleBandWidthDesign_ = 0;
        juce::String text_;
        ColourVariant colourVariant_ = ColourVariant::Blue;
        float uiScale_ = 1.0f;

        juce::AudioProcessorValueTreeState* apvts_ = nullptr;

        std::unique_ptr<Button> initButton_;
        std::unique_ptr<Button> copyButton_;
        std::unique_ptr<Button> pasteButton_;

        void drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds);

        juce::Colour getLineColour() const;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleHeader)
    };
}
