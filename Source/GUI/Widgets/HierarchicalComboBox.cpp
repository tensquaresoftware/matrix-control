#include "HierarchicalComboBox.h"

#include "ComboBoxControlPainter.h"
#include "HierarchicalPopupMenu.h"

namespace TSS
{
    HierarchicalComboBox::HierarchicalComboBox(int width, int height, const ComboBoxLook& look)
        : look_(look)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
    }

    HierarchicalComboBox::~HierarchicalComboBox() = default;

    void HierarchicalComboBox::setLook(const ComboBoxLook& look)
    {
        look_ = look;
        repaint();
    }

    void HierarchicalComboBox::setPopupMenuLook(const PopupMenuLook& look)
    {
        popupLook_ = look;
    }

    void HierarchicalComboBox::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void HierarchicalComboBox::setInactiveAppearance(bool inactive)
    {
        if (inactiveAppearance_ == inactive)
            return;

        inactiveAppearance_ = inactive;
        repaint();
    }

    void HierarchicalComboBox::clear()
    {
        primaryItems_.clear();
        selectedPrimaryId_ = 0;
        selectedChildId_ = 0;
        repaint();
    }

    void HierarchicalComboBox::addPrimaryItem(int id, const juce::String& label, bool isSentinel)
    {
        primaryItems_.push_back({ id, label, isSentinel, {} });
    }

    void HierarchicalComboBox::addChildItem(int primaryId, int id, const juce::String& label)
    {
        for (auto& primary : primaryItems_)
        {
            if (primary.id != primaryId || primary.isSentinel)
                continue;

            primary.children.push_back({ id, label });
            return;
        }
    }

    void HierarchicalComboBox::setSelectedPrimaryId(int primaryId, juce::NotificationType notification)
    {
        commitSelection(primaryId, 0, notification);
    }

    void HierarchicalComboBox::setSelectedChildId(int childId, juce::NotificationType notification)
    {
        commitSelection(selectedPrimaryId_, childId, notification);
    }

    void HierarchicalComboBox::setSelectedIds(int primaryId, int childId, juce::NotificationType notification)
    {
        commitSelection(primaryId, childId, notification);
    }

    void HierarchicalComboBox::setTextWhenNothingSelected(const juce::String& text)
    {
        textWhenNothingSelected_ = text;
        repaint();
    }

    void HierarchicalComboBox::paint(juce::Graphics& g)
    {
        const auto enabled = isEnabled() && ! inactiveAppearance_;
        ComboBoxControlPainter::paintClosedState(
            g,
            *this,
            getLocalBounds().toFloat(),
            ComboBoxControlStyle::Standard,
            look_,
            uiScale_,
            getDisplayText(),
            enabled,
            hasFocus_ || isPopupOpen_);
    }

    void HierarchicalComboBox::mouseDown(const juce::MouseEvent& e)
    {
        if (e.mods.isLeftButtonDown())
            showPopup();
    }

    void HierarchicalComboBox::focusGained(juce::Component::FocusChangeType)
    {
        if (isEnabled() && ! hasFocus_)
        {
            hasFocus_ = true;
            repaint();
        }
    }

    void HierarchicalComboBox::focusLost(juce::Component::FocusChangeType)
    {
        hasFocus_ = false;
        repaint();
    }

    bool HierarchicalComboBox::canShowPopupMenu() const
    {
        if (! isEnabled() || inactiveAppearance_)
            return false;

        for (const auto& primary : primaryItems_)
        {
            if (! primary.isSentinel)
                return true;
        }

        return false;
    }

    void HierarchicalComboBox::notifyPopupOpened()
    {
        isPopupOpen_ = true;
        repaint();
    }

    void HierarchicalComboBox::notifyPopupClosed()
    {
        isPopupOpen_ = false;
        repaint();
        grabKeyboardFocus();
    }

    void HierarchicalComboBox::commitSelectionFromPopup(int primaryId, int childId)
    {
        commitSelection(primaryId, childId, juce::sendNotification);
    }

    void HierarchicalComboBox::showPopup()
    {
        if (! canShowPopupMenu())
            return;

        HierarchicalPopupMenu::show(*this);
    }

    void HierarchicalComboBox::commitSelection(int primaryId, int childId, juce::NotificationType notification)
    {
        const auto* primary = findPrimaryItem(primaryId);
        if (primary == nullptr || primary->isSentinel)
            return;

        if (childId != 0 && findChildItem(*primary, childId) == nullptr)
            return;

        const bool changed = selectedPrimaryId_ != primaryId || selectedChildId_ != childId;
        selectedPrimaryId_ = primaryId;
        selectedChildId_ = childId;
        repaint();

        if (changed && notification != juce::dontSendNotification && onChange)
            onChange();
    }

    juce::String HierarchicalComboBox::getDisplayText() const
    {
        const auto* primary = findPrimaryItem(selectedPrimaryId_);
        if (primary == nullptr)
            return textWhenNothingSelected_;

        if (primary->isSentinel)
            return primary->label;

        if (selectedChildId_ != 0)
        {
            if (const auto* child = findChildItem(*primary, selectedChildId_))
            {
                const auto trimmedChildLabel = child->label.trim();
                if (trimmedChildLabel == "-" || trimmedChildLabel == juce::String::fromUTF8("\xe2\x80\x94"))
                    return primary->label;

                return primary->label + " " + child->label;
            }
        }

        return primary->label;
    }

    const HierarchicalComboBox::PrimaryItem* HierarchicalComboBox::findPrimaryItem(int primaryId) const
    {
        for (const auto& primary : primaryItems_)
        {
            if (primary.id == primaryId)
                return &primary;
        }

        return nullptr;
    }

    const HierarchicalComboBox::ChildItem* HierarchicalComboBox::findChildItem(const PrimaryItem& primary, int childId) const
    {
        for (const auto& child : primary.children)
        {
            if (child.id == childId)
                return &child;
        }

        return nullptr;
    }
}
