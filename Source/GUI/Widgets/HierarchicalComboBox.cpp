#include "HierarchicalComboBox.h"

#include "ComboBox.h"
#include "ComboBoxClosedControlHelper.h"
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
        ComboBoxClosedControlHelper::applyLook(look_, look, *this);
    }

    void HierarchicalComboBox::setPopupMenuLook(const PopupMenuLook& look)
    {
        ComboBoxClosedControlHelper::applyPopupMenuLook(popupLook_, look);
    }

    void HierarchicalComboBox::setUiScale(float uiScale)
    {
        ComboBoxClosedControlHelper::applyUiScale(uiScale_, uiScale, *this);
    }

    void HierarchicalComboBox::setInactiveAppearance(bool inactive)
    {
        if (inactiveAppearance_ == inactive)
            return;

        inactiveAppearance_ = inactive;
        repaint();
    }

    void HierarchicalComboBox::setPopupVerticalPlacement(PopupVerticalPlacement placement)
    {
        popupVerticalPlacement_ = placement;
    }

    int HierarchicalComboBox::getScaledVerticalMargin() const
    {
        return juce::roundToInt(static_cast<float>(ComboBox::getPopupLayoutDimensions().verticalMargin) * uiScale_);
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
            ComboBoxClosedControlHelper::shouldShowFocusRing(hasFocus_, isPopupOpen_));
    }

    void HierarchicalComboBox::mouseDown(const juce::MouseEvent& e)
    {
        if (e.mods.isLeftButtonDown())
            showPopup();
    }

    void HierarchicalComboBox::focusGained(juce::Component::FocusChangeType)
    {
        ComboBoxClosedControlHelper::applyFocusGained(hasFocus_, *this, isEnabled());
    }

    void HierarchicalComboBox::focusLost(juce::Component::FocusChangeType)
    {
        ComboBoxClosedControlHelper::applyFocusLost(hasFocus_, *this);
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
        ComboBoxClosedControlHelper::applyPopupOpened(isPopupOpen_, *this);
    }

    void HierarchicalComboBox::notifyPopupClosed()
    {
        ComboBoxClosedControlHelper::applyPopupClosed(isPopupOpen_, *this);
    }

    void HierarchicalComboBox::commitSelectionFromPopup(int primaryId, int childId)
    {
        commitSelection(primaryId, childId, juce::sendNotification);
    }

    void HierarchicalComboBox::showPopup()
    {
        if (! canShowPopupMenu())
            return;

        if (onBeforeShowPopup)
            onBeforeShowPopup();

        HierarchicalPopupMenu::show(*this);
    }

    void HierarchicalComboBox::commitSelection(int primaryId, int childId, juce::NotificationType notification)
    {
        const auto* primary = findPrimaryItem(primaryId);
        if (primary == nullptr || primary->isSentinel)
            return;

        // Fall back to primary-only when the requested child is missing (e.g. after a rebuild
        // where retries are still catching up) — never leave selection cleared at (0,0).
        if (childId != 0 && findChildItem(*primary, childId) == nullptr)
            childId = 0;

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

                // Full patch names (Mxx / Mxx-Ryy) replace the closed-control text.
                if (trimmedChildLabel == primary->label
                    || trimmedChildLabel.startsWith(primary->label + "-"))
                {
                    return trimmedChildLabel;
                }

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
