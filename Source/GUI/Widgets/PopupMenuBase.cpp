#include "PopupMenuBase.h"
#include "ComboBox.h"
#include "PopupMenuModalHelpers.h"

namespace TSS
{
    PopupMenuBase::PopupMenuBase(ComboBox& comboBox, bool isButtonLike)
        : host_(comboBox)
        , comboBox_(comboBox)
        , isButtonLike_(isButtonLike)
        , uiScale_(host_.getUiScale())
        , cachedFont_(host_.getPopupMenuLook().font
                        .withHeight(host_.getPopupMenuLook().font.getHeight() * uiScale_))
        , renderer_(isButtonLike, uiScale_)
    {
        renderer_.setLook(host_.getPopupMenuLook());
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        setOpaque(true);
    }

    PopupMenuBase::~PopupMenuBase() = default;

    int PopupMenuBase::getItemHeightDesign() const
    {
        return ComboBox::getPopupLayoutDimensions().itemHeight;
    }

    float PopupMenuBase::getBorderThicknessDesign() const
    {
        return static_cast<float>(ComboBox::getPopupLayoutDimensions().borderThickness);
    }

    void PopupMenuBase::mouseExit(const juce::MouseEvent&)
    {
        updateHighlightedItem(-1);
    }

    void PopupMenuBase::inputAttemptWhenModal()
    {
        closePopup();
    }

    bool PopupMenuBase::keyPressed(const juce::KeyPress& key)
    {
        if (PopupMenuModalHelpers::handleEscapeKey(key, *this, host_))
            return true;

        if (key.getKeyCode() == juce::KeyPress::returnKey)
        {
            if (highlightedItemIndex_ >= 0)
                selectItem(highlightedItemIndex_);
            return true;
        }

        handleKeyboardNavigation(key);

        return false;
    }

    void PopupMenuBase::updateHighlightedItem(int itemIndex)
    {
        if (highlightedItemIndex_ != itemIndex)
        {
            highlightedItemIndex_ = itemIndex;
            repaint();
        }
    }

    void PopupMenuBase::selectItem(int itemIndex)
    {
        const int selectedIndex = itemIndex;
        const bool valid = isValidItemIndex(selectedIndex);

        PopupMenuModalHelpers::dismissAndDelete(*this, host_, [this, selectedIndex, valid]()
        {
            if (! valid)
                return;

            const auto itemId = static_cast<juce::ComboBox&>(comboBox_).getItemId(selectedIndex);
            static_cast<juce::ComboBox&>(comboBox_).setSelectedId(itemId, juce::sendNotificationSync);
        });
    }

    void PopupMenuBase::closePopup()
    {
        PopupMenuModalHelpers::dismissAndDelete(*this, host_);
    }

    bool PopupMenuBase::isValidItemIndex(int itemIndex) const
    {
        return itemIndex >= 0 && itemIndex < static_cast<const juce::ComboBox&>(comboBox_).getNumItems();
    }
}
