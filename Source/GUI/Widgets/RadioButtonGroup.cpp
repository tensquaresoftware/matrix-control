#include "RadioButtonGroup.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"

namespace TSS
{
    namespace
    {
        constexpr int kOptionWidthDesign = 64;
        constexpr int kOptionHeightDesign = 20;
        constexpr int kGapXDesign = 8;
        constexpr int kGapYDesign = 4;
    }

    RadioButtonGroup::OptionButton::OptionButton(const juce::String& text)
        : juce::Button(text)
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        setClickingTogglesState(false);
    }

    void RadioButtonGroup::OptionButton::setLook(const Look& look)
    {
        look_ = look;
        repaint();
    }

    void RadioButtonGroup::OptionButton::setUiScale(float uiScale)
    {
        uiScale_ = uiScale;
        repaint();
    }

    void RadioButtonGroup::OptionButton::setSelected(bool isSelected)
    {
        if (selected_ == isSelected)
            return;

        selected_ = isSelected;
        repaint();
    }

    void RadioButtonGroup::OptionButton::paintButton(juce::Graphics& g,
                                                     bool /*shouldDrawButtonAsHighlighted*/,
                                                     bool /*shouldDrawButtonAsDown*/)
    {
        const int square = ScaledLayout::scaledInt(static_cast<float>(kSquareDesign_), uiScale_);
        const int gap = ScaledLayout::scaledInt(static_cast<float>(kSquareTextGapDesign_), uiScale_);
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(kBorderThickness_),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);

        auto bounds = getLocalBounds();
        const int squareY = bounds.getY() + (bounds.getHeight() - square) / 2;
        const auto squareBounds = juce::Rectangle<float>(
            static_cast<float>(bounds.getX()),
            static_cast<float>(squareY),
            static_cast<float>(square),
            static_cast<float>(square));

        // Nested fills use the same snapped float thickness as ButtonLike combo borders,
        // so the black ring matches the grey border at every UI Scale (incl. 75% -> 1.5).
        const auto borderColour = isEnabled() ? look_.border : look_.borderDisabled;

        g.setColour(borderColour);
        g.fillRect(squareBounds);

        const auto inner = squareBounds.reduced(borderThickness);
        if (inner.getWidth() > 0.0f && inner.getHeight() > 0.0f)
        {
            g.setColour(look_.squareOff);
            g.fillRect(inner);

            if (selected_)
            {
                const auto onBounds = squareBounds.reduced(borderThickness * 2.0f);
                if (onBounds.getWidth() > 0.0f && onBounds.getHeight() > 0.0f)
                {
                    g.setColour(look_.squareOn);
                    g.fillRect(onBounds);
                }
            }
        }

        auto textBounds = bounds.withTrimmedLeft(square + gap);
        g.setColour(isEnabled() ? look_.text : look_.textDisabled);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
        g.drawText(getButtonText(), textBounds, juce::Justification::centredLeft, false);
    }

    RadioButtonGroup::RadioButtonGroup()
    {
        setOpaque(false);
    }

    RadioButtonGroup::Look RadioButtonGroup::lookFromSkin(ISkin& skin) const
    {
        const auto label = labelLookFromSkin(skin);
        Look look;
        look.squareOff = skin.getColour(SkinColourId::kButtonBackgroundOff);
        look.squareOn = label.text;
        look.border = skin.getColour(SkinColourId::kButtonBorderOff);
        look.borderDisabled = skin.getColour(SkinColourId::kButtonBorderDisabled);
        look.text = label.text;
        look.textDisabled = skin.getColour(SkinColourId::kButtonTextDisabled);
        look.font = label.font;
        return look;
    }

    void RadioButtonGroup::setSkin(ISkin& skin)
    {
        skin_ = &skin;
        look_ = lookFromSkin(skin);
        applyLooks();
        repaint();
    }

    void RadioButtonGroup::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        applyLooks();
        resized();
        repaint();
    }

    void RadioButtonGroup::setOptions(const juce::StringArray& labels)
    {
        labels_ = labels;
        rebuildButtons();
        if (selectedIndex_ >= getOptionCount())
            selectedIndex_ = getOptionCount() > 0 ? 0 : -1;

        for (int i = 0; i < getOptionCount(); ++i)
            options_[static_cast<size_t>(i)]->setSelected(i == selectedIndex_);

        resized();
        repaint();
    }

    void RadioButtonGroup::setSelectedIndex(int index, juce::NotificationType notification)
    {
        if (index < 0 || index >= getOptionCount())
            index = -1;

        if (selectedIndex_ == index)
            return;

        selectedIndex_ = index;
        for (int i = 0; i < getOptionCount(); ++i)
            options_[static_cast<size_t>(i)]->setSelected(i == selectedIndex_);

        if (notification != juce::dontSendNotification && onSelectionChanged)
            onSelectionChanged();
    }

    int RadioButtonGroup::getPreferredHeight(int availableWidth) const noexcept
    {
        return RadioButtonGroupLayout::preferredHeight(getOptionCount(), availableWidth, optionMetrics());
    }

    void RadioButtonGroup::resized()
    {
        const auto metrics = optionMetrics();
        const auto area = getLocalBounds();
        for (int i = 0; i < getOptionCount(); ++i)
        {
            options_[static_cast<size_t>(i)]->setBounds(
                RadioButtonGroupLayout::optionBounds(i, getOptionCount(), area, metrics));
            options_[static_cast<size_t>(i)]->setUiScale(uiScale_);
        }
    }

    void RadioButtonGroup::paint(juce::Graphics&)
    {
    }

    void RadioButtonGroup::rebuildButtons()
    {
        options_.clear();
        options_.reserve(static_cast<size_t>(labels_.size()));

        for (int i = 0; i < labels_.size(); ++i)
        {
            auto button = std::make_unique<OptionButton>(labels_[i]);
            const int index = i;
            button->onClick = [this, index]
            {
                setSelectedIndex(index, juce::sendNotification);
            };
            addAndMakeVisible(*button);
            options_.push_back(std::move(button));
        }

        applyLooks();
    }

    void RadioButtonGroup::applyLooks()
    {
        if (skin_ != nullptr)
            look_ = lookFromSkin(*skin_);

        for (auto& option : options_)
        {
            option->setLook(look_);
            option->setUiScale(uiScale_);
        }
    }

    RadioButtonGroupLayout::OptionMetrics RadioButtonGroup::optionMetrics() const noexcept
    {
        return {
            .optionWidth = ScaledLayout::scaledInt(static_cast<float>(kOptionWidthDesign), uiScale_),
            .optionHeight = ScaledLayout::scaledInt(static_cast<float>(kOptionHeightDesign), uiScale_),
            .gapX = ScaledLayout::scaledInt(static_cast<float>(kGapXDesign), uiScale_),
            .gapY = ScaledLayout::scaledInt(static_cast<float>(kGapYDesign), uiScale_),
        };
    }
}
