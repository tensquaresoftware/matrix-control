#include "PopupMenuBase.h"
#include "ComboBox.h"

namespace tss
{
    PopupMenuBase::PopupMenuBase(ComboBox& comboBox, bool isButtonLike)
        : comboBox_(comboBox)
        , isButtonLike_(isButtonLike)
        , uiScale_(comboBox.getUiScale())
        , cachedFont_(comboBox.getPopupMenuLook().font
                        .withHeight(comboBox.getPopupMenuLook().font.getHeight() * uiScale_))
        , renderer_(isButtonLike, uiScale_)
    {
        renderer_.setLook(comboBox_.getPopupMenuLook());
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        setOpaque(true);
    }

    PopupMenuBase::~PopupMenuBase() = default;

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
        if (key.getKeyCode() == juce::KeyPress::escapeKey)
        {
            closePopup();
            return true;
        }
        
        if (key.getKeyCode() == juce::KeyPress::returnKey)
        {
            if (highlightedItemIndex_ >= 0)
            {
                selectItem(highlightedItemIndex_);
            }
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
        exitModalState(0);
        
        comboBox_.isPopupOpen_ = false;
        static_cast<juce::Component&>(comboBox_).repaint();
        
        if (isValidItemIndex(itemIndex))
        {
            const auto itemId = static_cast<juce::ComboBox&>(comboBox_).getItemId(itemIndex);
            static_cast<juce::ComboBox&>(comboBox_).setSelectedId(itemId, juce::sendNotificationSync);
        }
        
        static_cast<juce::Component&>(comboBox_).grabKeyboardFocus();
        
        if (auto* parent = getParentComponent())
        {
            parent->removeChildComponent(this);
        }
        
        delete this;
    }

    void PopupMenuBase::closePopup()
    {
        exitModalState(0);
        
        comboBox_.isPopupOpen_ = false;
        static_cast<juce::Component&>(comboBox_).repaint();
        
        static_cast<juce::Component&>(comboBox_).grabKeyboardFocus();
        
        if (auto* parent = getParentComponent())
        {
            parent->removeChildComponent(this);
        }
        
        delete this;
    }

    bool PopupMenuBase::isValidItemIndex(int itemIndex) const
    {
        return itemIndex >= 0 && itemIndex < static_cast<const juce::ComboBox&>(comboBox_).getNumItems();
    }
}
