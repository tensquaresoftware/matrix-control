#include "ComboBox.h"
#include "ComboBoxClosedControlHelper.h"
#include "ComboBoxControlPainter.h"
#include "MultiColumnPopupMenu.h"
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
            *this,
            getLocalBounds().toFloat(),
            style,
            look_,
            uiScale_,
            getSelectedItemText(),
            isEnabled(),
            ComboBoxClosedControlHelper::shouldShowFocusRing(hasFocus_, isPopupOpen_));
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

        juce::MessageManager::callAsync([safePointer = SafePointer<ComboBox>(this), useScrollableMode]()
        {
            if (safePointer != nullptr && safePointer->canShowPopup())
            {
                safePointer->notifyPopupOpened();

                if (useScrollableMode)
                    ScrollablePopupMenu::show(*safePointer);
                else
                    MultiColumnPopupMenu::show(*safePointer);
            }
        });
    }

    void ComboBox::notifyPopupOpened()
    {
        ComboBoxClosedControlHelper::applyPopupOpened(isPopupOpen_, *this);
    }

    void ComboBox::notifyPopupClosed()
    {
        ComboBoxClosedControlHelper::applyPopupClosed(isPopupOpen_, *this);
    }

    void ComboBox::mouseDown(const juce::MouseEvent& e)
    {
        if (isEnabled())
        {
            if (e.mods.isLeftButtonDown())
                showPopup();
        }
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
