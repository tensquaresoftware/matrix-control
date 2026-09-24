#include "ComboBox.h"
#include "ComboBoxClosedControlHelper.h"
#include "ComboBoxControlPainter.h"
#include "MultiColumnPopupMenu.h"
#include "PopupMenuModalHelpers.h"
#include "ScrollablePopupMenu.h"

#include "GUI/Skins/ColourChart.h"

namespace TSS
{
    namespace
    {
        PopupMenuLayoutDimensions defaultPopupLayoutDimensions()
        {
            return {
                .itemHeight = 20,
                .verticalMargin = 4,
                .scrollbarWidth = 8,
                .minThumbHeight = 20,
                .maxScrollHeight = 200,
                .borderThickness = 1,
                .textLeftPadding = 3,
            };
        }
    }

    PopupMenuLayoutDimensions ComboBox::popupLayoutDimensions_ = defaultPopupLayoutDimensions();

    void ComboBox::setPopupLayoutDimensions(const PopupMenuLayoutDimensions& dimensions)
    {
        popupLayoutDimensions_ = dimensions;
    }

    const PopupMenuLayoutDimensions& ComboBox::getPopupLayoutDimensions()
    {
        return popupLayoutDimensions_;
    }

    int ComboBox::getScaledVerticalMargin() const
    {
        return juce::roundToInt(static_cast<float>(popupLayoutDimensions_.verticalMargin) * uiScale_);
    }

    ComboBox::ComboBox(int width, int height, const ComboBoxLook& look, Style style)
        : juce::ComboBox()
        , look_(look)
        , width_(width)
        , height_(height)
        , style_(style)
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
        setColour(juce::ComboBox::textColourId, juce::Colour(ColourChart::kTransparent));
    }

    void ComboBox::setLook(const ComboBoxLook& look)
    {
        ComboBoxClosedControlHelper::applyLook(look_, look, *this);
    }

    void ComboBox::setPopupMenuLook(const PopupMenuLook& look)
    {
        ComboBoxClosedControlHelper::applyPopupMenuLook(popupLook_, look);
    }

    void ComboBox::setUiScale(float uiScale)
    {
        ComboBoxClosedControlHelper::applyUiScale(uiScale_, uiScale, *this);
    }

    void ComboBox::setPopupVerticalPlacement(PopupVerticalPlacement placement)
    {
        popupVerticalPlacement_ = placement;
    }

    void ComboBox::paint(juce::Graphics& g)
    {
        const auto style = style_ == Style::ButtonLike
            ? ComboBoxControlStyle::ButtonLike
            : ComboBoxControlStyle::Standard;

        ComboBoxControlPainter::paintClosedState(
            g,
            {
                .component = *this,
                .bounds = getLocalBounds().toFloat(),
                .style = style,
                .look = look_,
                .uiScale = uiScale_,
                .text = getSelectedItemText(),
                .enabled = isEnabled(),
                .hasFocus = ComboBoxClosedControlHelper::shouldShowFocusRing(hasFocus_, isPopupOpen_),
            });
    }

    juce::String ComboBox::getSelectedItemText() const
    {
        const auto selectedIndex = getSelectedItemIndex();
        if (selectedIndex >= 0)
            return getItemText(selectedIndex);

        return getTextWhenNothingSelected();
    }

    void ComboBox::showPopup()
    {
        showPopupInternal(true);
    }

    void ComboBox::showPopupAfterItemRebuild()
    {
        showPopupInternal(false);
    }

    void ComboBox::showPopupInternal(bool invokeAboutToShow)
    {
        if (! canShowPopup())
            return;

        if (invokeAboutToShow && onAboutToShowPopup)
            onAboutToShowPopup();

        if (! canShowPopup())
            return;

        showPopupAsynchronously();
        repaint();
    }

    bool ComboBox::canShowPopup() const
    {
        return isEnabled() && getNumItems() > 0;
    }

    void ComboBox::showPopupAsynchronously()
    {
        const auto useScrollableMode = (style_ == Style::ButtonLike);
        const auto generation = ++popupShowGeneration_;

        juce::MessageManager::callAsync(
            [safePointer = SafePointer<ComboBox>(this), useScrollableMode, generation]()
            {
                if (safePointer == nullptr || safePointer->popupShowGeneration_ != generation)
                    return;

                if (! safePointer->canShowPopup())
                    return;

                // Open notify happens inside ::show after empty/null guards (Hierarchical order).
                if (useScrollableMode)
                    ScrollablePopupMenu::show(*safePointer);
                else
                    MultiColumnPopupMenu::show(*safePointer);
            });
    }

    void ComboBox::notifyPopupOpened()
    {
        ComboBoxClosedControlHelper::applyPopupOpened(isPopupOpen_, *this);
    }

    void ComboBox::notifyPopupClosed()
    {
        activePopup_ = nullptr;
        ++popupShowGeneration_;
        ComboBoxClosedControlHelper::applyPopupClosed(isPopupOpen_, *this);
    }

    void ComboBox::attachOpenPopup(juce::Component& popup)
    {
        activePopup_ = &popup;
    }

    void ComboBox::dismissPopup()
    {
        if (auto* popup = activePopup_.getComponent())
        {
            PopupMenuModalHelpers::dismissAndDelete(*popup, *this);
            return;
        }

        if (isPopupOpen_)
            notifyPopupClosed();
    }

    void ComboBox::suppressNextPopupOpen()
    {
        ComboBoxClosedControlHelper::armSuppressNextPopupOpen(suppressNextPopupOpen_);
    }

    void ComboBox::mouseDown(const juce::MouseEvent& e)
    {
        // Consume before enablement / button checks so a sticky arm cannot block a later open.
        if (ComboBoxClosedControlHelper::consumeSuppressNextPopupOpen(suppressNextPopupOpen_))
            return;

        if (! isEnabled())
            return;

        if (! e.mods.isLeftButtonDown())
            return;

        showPopup();
    }

    void ComboBox::focusGained(juce::Component::FocusChangeType)
    {
        ComboBoxClosedControlHelper::applyFocusGained(hasFocus_, *this, isEnabled());
    }

    void ComboBox::focusLost(juce::Component::FocusChangeType)
    {
        ComboBoxClosedControlHelper::applyFocusLost(hasFocus_, *this);
    }
}
