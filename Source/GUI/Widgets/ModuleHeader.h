#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Looks/WidgetLooks.h"

class WidgetFactory;

namespace TSS
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
            const ModuleHeaderDimensions& dimensions;
            juce::String moduleId;
            ColumnLayout columnLayout;
            ButtonSet buttonSet;
            juce::String initWidgetId;
            juce::String copyWidgetId;
            juce::String pasteWidgetId;
        };

        explicit ModuleHeader(int width, int height, const ModuleHeaderLook& look, ColourVariant variant,
                              const juce::String& text, const ModuleHeaderDimensions& dimensions);

        explicit ModuleHeader(const WithActionsSpec& spec);

        ~ModuleHeader() override;

        void setLook(const ModuleHeaderLook& look);
        void setSkin(ISkin& skin);
        void setUiScale(float uiScale);

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

        ModuleHeaderLook look_{};
        ModuleHeaderDimensions dimensions_{};
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
