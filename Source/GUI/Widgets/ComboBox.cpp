#include "ComboBox.h"
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
                .maxScrollHeight = 300,
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
        look_ = look;
        repaint();
    }

    void ComboBox::setPopupMenuLook(const PopupMenuLook& look)
    {
        popupLook_ = look;
    }

    void ComboBox::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
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
            hasFocus_ || isPopupOpen_);
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
                safePointer->isPopupOpen_ = true;
                
                if (useScrollableMode)
                {
                    ScrollablePopupMenu::show(*safePointer);
                }
                else
                {
                    MultiColumnPopupMenu::show(*safePointer);
                }
            }
        });
    }

    void ComboBox::mouseDown(const juce::MouseEvent& e)
    {
        if (isEnabled())
        {
            if (e.mods.isLeftButtonDown())
            {
                showPopup();
            }
        }
    }

    void ComboBox::focusGained(juce::Component::FocusChangeType)
    {
        if (isEnabled() && ! hasFocus_)
        {
            hasFocus_ = true;
            repaint();
        }
    }

    void ComboBox::focusLost(juce::Component::FocusChangeType)
    {
        hasFocus_ = false;
        repaint();
    }
}
