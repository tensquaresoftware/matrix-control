#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"

class SkinColourSubstrateTests;

namespace TSS
{
    /** Read-only value strip for dialogs (label-aligned control height). */
    class ReadOnlyValueField : public juce::Component
    {
    public:
        explicit ReadOnlyValueField(ISkin& skin, ColourSet colourSet = ColourSet::k1)
            : skin_(&skin)
            , colourSet_(colourSet)
        {
            setInterceptsMouseClicks(false, false);
            applyDefaultColoursFromSkin();
        }

        void setColourSet(ColourSet colourSet)
        {
            if (colourSet_ == colourSet)
                return;

            colourSet_ = colourSet;
            applyDefaultColoursFromSkin();
            repaint();
        }

        void setSkin(ISkin& skin)
        {
            skin_ = &skin;
            applyDefaultColoursFromSkin();
            repaint();
        }

        ColourSet getColourSet() const noexcept { return colourSet_; }

        void setUiScale(float uiScale)
        {
            if (juce::approximatelyEqual(uiScale_, uiScale))
                return;

            uiScale_ = uiScale;
            repaint();
        }

        void setText(const juce::String& text)
        {
            if (text_ == text)
                return;

            text_ = text;
            repaint();
        }

        void setColours(juce::Colour fill, juce::Colour text)
        {
            fill_ = fill;
            textColour_ = text;
            repaint();
        }

        void paint(juce::Graphics& g) override
        {
            g.fillAll(fill_);

            if (skin_ == nullptr || text_.isEmpty())
                return;

            g.setColour(textColour_);
            g.setFont(skin_->getBaseFont().withHeight(skin_->getBaseFont().getHeight() * uiScale_));
            g.drawText(text_,
                       getLocalBounds().reduced(juce::roundToInt(6.0f * uiScale_), 0),
                       juce::Justification::centredLeft,
                       true);
        }

    private:
        friend class ::SkinColourSubstrateTests;

        void applyDefaultColoursFromSkin()
        {
            if (skin_ == nullptr)
                return;

            fill_ = skin_->getColour(SkinObjectType::kReadOnlyValueField,
                                     SkinColourProperty::kBackground,
                                     SkinColourState::kDefault,
                                     colourSet_);
            textColour_ = skin_->getColour(SkinObjectType::kReadOnlyValueField,
                                           SkinColourProperty::kText,
                                           SkinColourState::kDefault,
                                           colourSet_);
        }

        ISkin* skin_ = nullptr;
        ColourSet colourSet_ = ColourSet::k1;
        juce::String text_;
        juce::Colour fill_{};
        juce::Colour textColour_{};
        float uiScale_ = 1.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReadOnlyValueField)
    };
}
