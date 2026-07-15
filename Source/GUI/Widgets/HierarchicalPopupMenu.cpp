#include "HierarchicalPopupMenu.h"

#include "ComboBox.h"
#include "HierarchicalComboBox.h"
#include "PopupMenuPositioner.h"

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    int HierarchicalPopupMenu::countOpenablePrimaries(const HierarchicalComboBox& owner)
    {
        int count = 0;
        for (int i = 0; i < owner.getPrimaryItemCount(); ++i)
        {
            if (! owner.getPrimaryItem(i).isSentinel)
                ++count;
        }
        return count;
    }

    int HierarchicalPopupMenu::openableIndexForPrimaryId(const HierarchicalComboBox& owner, int primaryId)
    {
        int openableIndex = 0;
        for (int i = 0; i < owner.getPrimaryItemCount(); ++i)
        {
            const auto& primary = owner.getPrimaryItem(i);
            if (primary.isSentinel)
                continue;

            if (primary.id == primaryId)
                return openableIndex;

            ++openableIndex;
        }

        return -1;
    }

    int HierarchicalPopupMenu::primaryStorageIndexForOpenableIndex(const HierarchicalComboBox& owner, int openableIndex)
    {
        int current = 0;
        for (int i = 0; i < owner.getPrimaryItemCount(); ++i)
        {
            if (owner.getPrimaryItem(i).isSentinel)
                continue;

            if (current == openableIndex)
                return i;

            ++current;
        }

        return -1;
    }

    HierarchicalPopupMenu::HierarchicalPopupMenu(HierarchicalComboBox& owner)
        : owner_(owner)
        , renderer_(false, owner.getUiScale())
        , uiScale_(owner.getUiScale())
        , cachedFont_(owner.getPopupMenuLook().font.withHeight(owner.getPopupMenuLook().font.getHeight() * uiScale_))
        , primaryColumnWidth_(static_cast<float>(owner.getBaseComponentWidth()) * uiScale_)
        , secondaryColumnWidth_(static_cast<float>(owner.getBaseComponentWidth()) * uiScale_)
    {
        renderer_.setLook(owner.getPopupMenuLook());
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        setOpaque(false);

        highlightedPrimaryIndex_ = openableIndexForPrimaryId(owner_, owner_.getSelectedPrimaryId());
        if (highlightedPrimaryIndex_ < 0 && countOpenablePrimaries(owner_) > 0)
            highlightedPrimaryIndex_ = 0;

        if (highlightedPrimaryIndex_ >= 0)
        {
            const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
            if (storageIndex >= 0)
            {
                const auto& primary = owner_.getPrimaryItem(storageIndex);
                for (size_t i = 0; i < primary.children.size(); ++i)
                {
                    if (primary.children[i].id == owner_.getSelectedChildId())
                    {
                        highlightedChildIndex_ = static_cast<int>(i);
                        break;
                    }
                }
            }
        }
    }

    HierarchicalPopupMenu::~HierarchicalPopupMenu() = default;

    bool HierarchicalPopupMenu::hasSecondaryColumn() const
    {
        if (highlightedPrimaryIndex_ < 0)
            return false;

        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        if (storageIndex < 0)
            return false;

        return ! owner_.getPrimaryItem(storageIndex).children.empty();
    }

    int HierarchicalPopupMenu::getOpenablePrimaryCount() const
    {
        return countOpenablePrimaries(owner_);
    }

    float HierarchicalPopupMenu::getItemHeight() const
    {
        return static_cast<float>(ComboBox::getPopupLayoutDimensions().itemHeight) * uiScale_;
    }

    float HierarchicalPopupMenu::getBorderThicknessDesign() const
    {
        return static_cast<float>(ComboBox::getPopupLayoutDimensions().borderThickness);
    }

    float HierarchicalPopupMenu::getLayoutBorderThickness() const
    {
        return juce::jmax(1.0f, getBorderThicknessDesign() * uiScale_);
    }

    float HierarchicalPopupMenu::getSnappedBorderThickness() const
    {
        return ScaledDrawing::snappedStrokeThicknessFromDesign(
            getBorderThicknessDesign(),
            uiScale_,
            ScaledDrawing::systemDisplayScaleForComponent(*this),
            ScaledDrawing::StrokeSnapPolicy::kRound);
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getPrimaryPanelBounds() const
    {
        const float border = getLayoutBorderThickness();
        const float itemHeight = getItemHeight();
        const float height = static_cast<float>(getOpenablePrimaryCount()) * itemHeight + 2.0f * border;

        return {
            0.0f,
            0.0f,
            primaryColumnWidth_ + 2.0f * border,
            height
        };
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getSecondaryPanelBounds() const
    {
        if (! hasSecondaryColumn() || highlightedPrimaryIndex_ < 0)
            return {};

        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        if (storageIndex < 0)
            return {};

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        const float border = getLayoutBorderThickness();
        const float itemHeight = getItemHeight();
        const auto primaryPanel = getPrimaryPanelBounds();
        const float panelTop = primaryPanel.getY()
                             + static_cast<float>(highlightedPrimaryIndex_) * itemHeight;
        const float panelHeight = static_cast<float>(primary.children.size()) * itemHeight + 2.0f * border;

        return {
            primaryPanel.getRight() - border,
            panelTop,
            secondaryColumnWidth_ + 2.0f * border,
            panelHeight
        };
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getPrimaryContentBounds() const
    {
        return getPrimaryPanelBounds().reduced(getLayoutBorderThickness());
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getSecondaryContentBounds() const
    {
        const auto panel = getSecondaryPanelBounds();
        if (panel.isEmpty())
            return {};

        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        if (storageIndex < 0)
            return {};

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        const float border = getLayoutBorderThickness();

        return {
            panel.getX() + border,
            panel.getY() + border,
            secondaryColumnWidth_,
            static_cast<float>(primary.children.size()) * getItemHeight()
        };
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getPrimaryItemBounds(int primaryIndex) const
    {
        const auto contentBounds = getPrimaryContentBounds();
        const float itemHeight = getItemHeight();
        return {
            contentBounds.getX(),
            contentBounds.getY() + static_cast<float>(primaryIndex) * itemHeight,
            primaryColumnWidth_,
            itemHeight
        };
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getSecondaryItemBounds(int childIndex) const
    {
        const auto contentBounds = getSecondaryContentBounds();
        if (contentBounds.isEmpty())
            return {};

        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        if (storageIndex < 0)
            return {};

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        if (! juce::isPositiveAndBelow(childIndex, static_cast<int>(primary.children.size())))
            return {};

        const float itemHeight = getItemHeight();
        return {
            contentBounds.getX(),
            contentBounds.getY() + static_cast<float>(childIndex) * itemHeight,
            secondaryColumnWidth_,
            itemHeight
        };
    }

    void HierarchicalPopupMenu::getPreferredContentSize(float& width, float& height) const
    {
        const auto primaryPanel = getPrimaryPanelBounds();
        width = primaryPanel.getWidth();
        height = primaryPanel.getHeight();

        if (hasSecondaryColumn())
        {
            const auto secondaryPanel = getSecondaryPanelBounds();
            width = secondaryPanel.getRight();
            height = juce::jmax(primaryPanel.getHeight(), secondaryPanel.getBottom());
        }
    }

    int HierarchicalPopupMenu::getPrimaryIndexAt(int x, int y) const
    {
        const auto contentBounds = getPrimaryContentBounds();
        if (! contentBounds.contains(static_cast<float>(x), static_cast<float>(y)))
            return -1;

        const int row = static_cast<int>((static_cast<float>(y) - contentBounds.getY()) / getItemHeight());
        if (row < 0 || row >= getOpenablePrimaryCount())
            return -1;

        return row;
    }

    int HierarchicalPopupMenu::getChildIndexAt(int x, int y) const
    {
        if (! hasSecondaryColumn())
            return -1;

        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        if (storageIndex < 0)
            return -1;

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        for (int childIndex = 0; childIndex < static_cast<int>(primary.children.size()); ++childIndex)
        {
            const auto bounds = getSecondaryItemBounds(childIndex);
            if (bounds.contains(static_cast<float>(x), static_cast<float>(y)))
                return childIndex;
        }

        return -1;
    }

    void HierarchicalPopupMenu::updateHighlightFromPosition(int x, int y)
    {
        const auto childIndex = getChildIndexAt(x, y);
        if (childIndex >= 0)
        {
            highlightedChildIndex_ = childIndex;
            repaint();
            return;
        }

        const auto primaryIndex = getPrimaryIndexAt(x, y);
        if (primaryIndex >= 0 && primaryIndex != highlightedPrimaryIndex_)
        {
            highlightedPrimaryIndex_ = primaryIndex;
            highlightedChildIndex_ = -1;
            repaint();
        }
    }

    void HierarchicalPopupMenu::selectPrimaryLeaf(int primaryIndex)
    {
        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, primaryIndex);
        if (storageIndex < 0)
            return;

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        if (! primary.children.empty())
            return;

        owner_.commitSelectionFromPopup(primary.id, 0);
        closePopup();
    }

    void HierarchicalPopupMenu::selectChild(int primaryIndex, int childIndex)
    {
        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, primaryIndex);
        if (storageIndex < 0)
            return;

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        if (! juce::isPositiveAndBelow(childIndex, static_cast<int>(primary.children.size())))
            return;

        owner_.commitSelectionFromPopup(primary.id, primary.children[static_cast<size_t>(childIndex)].id);
        closePopup();
    }

    void HierarchicalPopupMenu::closePopup()
    {
        exitModalState(0);
        owner_.notifyPopupClosed();

        if (auto* parent = getParentComponent())
            parent->removeChildComponent(this);

        delete this;
    }

    void HierarchicalPopupMenu::drawPanelBorderEdges(juce::Graphics& g,
                                                     juce::Rectangle<float> panel,
                                                     float thickness,
                                                     bool drawLeft,
                                                     bool drawTop,
                                                     bool drawRight,
                                                     bool drawBottom) const
    {
        if (drawTop)
            g.fillRect(panel.getX(), panel.getY(), panel.getWidth(), thickness);

        if (drawBottom)
            g.fillRect(panel.getX(), panel.getBottom() - thickness, panel.getWidth(), thickness);

        if (drawLeft)
            g.fillRect(panel.getX(), panel.getY(), thickness, panel.getHeight());

        if (drawRight)
            g.fillRect(panel.getRight() - thickness, panel.getY(), thickness, panel.getHeight());
    }

    void HierarchicalPopupMenu::drawStaircasePanelBorders(juce::Graphics& g,
                                                            juce::Rectangle<float> primaryPanel,
                                                            juce::Rectangle<float> secondaryPanel,
                                                            float thickness) const
    {
        drawPanelBorderEdges(g, primaryPanel, thickness, true, true, false, true);

        const float sharedX = primaryPanel.getRight() - thickness;
        const float upperRightHeight = juce::jmax(0.0f, secondaryPanel.getY() - primaryPanel.getY());
        if (upperRightHeight > 0.0f)
            g.fillRect(sharedX, primaryPanel.getY(), thickness, upperRightHeight);

        const float lowerRightTop = secondaryPanel.getBottom();
        const float lowerRightHeight = juce::jmax(0.0f, primaryPanel.getBottom() - lowerRightTop);
        if (lowerRightHeight > 0.0f)
            g.fillRect(sharedX, lowerRightTop, thickness, lowerRightHeight);

        drawPanelBorderEdges(g, secondaryPanel, thickness, true, true, true, true);
    }

    bool HierarchicalPopupMenu::hitTest(int x, int y)
    {
        const auto primaryPanel = getPrimaryPanelBounds();
        if (primaryPanel.contains(static_cast<float>(x), static_cast<float>(y)))
            return true;

        if (hasSecondaryColumn())
        {
            const auto secondaryPanel = getSecondaryPanelBounds();
            if (secondaryPanel.contains(static_cast<float>(x), static_cast<float>(y)))
                return true;
        }

        return false;
    }

    void HierarchicalPopupMenu::paint(juce::Graphics& g)
    {
        const auto primaryPanel = getPrimaryPanelBounds();
        const auto popupLook = owner_.getPopupMenuLook();

        renderer_.drawBackground(g, primaryPanel);
        for (int primaryIndex = 0; primaryIndex < getOpenablePrimaryCount(); ++primaryIndex)
        {
            const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, primaryIndex);
            if (storageIndex < 0)
                continue;

            const auto& primary = owner_.getPrimaryItem(storageIndex);
            const auto itemBounds = getPrimaryItemBounds(primaryIndex);
            const bool isHighlighted = primaryIndex == highlightedPrimaryIndex_;
            renderer_.drawLabelItem(g, primary.label, itemBounds, isHighlighted, true, cachedFont_);

            if (! primary.children.empty())
                renderer_.drawSubMenuChevron(g, itemBounds, cachedFont_);
        }

        g.setColour(popupLook.border);
        const float border = getLayoutBorderThickness();

        if (hasSecondaryColumn())
        {
            const auto secondaryPanel = getSecondaryPanelBounds();
            const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
            const auto& primary = owner_.getPrimaryItem(storageIndex);

            renderer_.drawBackground(g, secondaryPanel);
            for (size_t childIndex = 0; childIndex < primary.children.size(); ++childIndex)
            {
                const auto itemBounds = getSecondaryItemBounds(static_cast<int>(childIndex));
                const bool isHighlighted = highlightedChildIndex_ == static_cast<int>(childIndex);
                renderer_.drawLabelItem(g,
                                        primary.children[childIndex].label,
                                        itemBounds,
                                        isHighlighted,
                                        true,
                                        cachedFont_);
            }

            drawStaircasePanelBorders(g, primaryPanel, secondaryPanel, border);
        }
        else
        {
            drawPanelBorderEdges(g, primaryPanel, border, true, true, true, true);
        }
    }

    void HierarchicalPopupMenu::mouseMove(const juce::MouseEvent& e)
    {
        const auto previousPrimary = highlightedPrimaryIndex_;
        const auto hadSecondary = hasSecondaryColumn();
        updateHighlightFromPosition(e.getPosition().x, e.getPosition().y);

        if (highlightedPrimaryIndex_ != previousPrimary || hasSecondaryColumn() != hadSecondary)
        {
            float width = 0.0f;
            float height = 0.0f;
            getPreferredContentSize(width, height);
            setSize(juce::roundToInt(width), juce::roundToInt(height));
            repaint();
        }
    }

    void HierarchicalPopupMenu::mouseUp(const juce::MouseEvent& e)
    {
        const auto childIndex = getChildIndexAt(e.getPosition().x, e.getPosition().y);
        if (childIndex >= 0)
        {
            selectChild(highlightedPrimaryIndex_, childIndex);
            return;
        }

        const auto primaryIndex = getPrimaryIndexAt(e.getPosition().x, e.getPosition().y);
        if (primaryIndex >= 0)
            selectPrimaryLeaf(primaryIndex);
    }

    void HierarchicalPopupMenu::mouseExit(const juce::MouseEvent&)
    {
    }

    void HierarchicalPopupMenu::inputAttemptWhenModal()
    {
        closePopup();
    }

    bool HierarchicalPopupMenu::keyPressed(const juce::KeyPress& key)
    {
        if (key.getKeyCode() == juce::KeyPress::escapeKey)
        {
            closePopup();
            return true;
        }

        return false;
    }

    void HierarchicalPopupMenu::show(HierarchicalComboBox& owner)
    {
        if (! owner.canShowPopupMenu())
            return;

        auto* topLevelComponent = owner.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return;

        owner.notifyPopupOpened();

        auto popupMenu = std::make_unique<HierarchicalPopupMenu>(owner);
        auto* rawPtr = popupMenu.get();

        float contentWidth = 0.0f;
        float contentHeight = 0.0f;
        rawPtr->getPreferredContentSize(contentWidth, contentHeight);

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            owner,
            juce::roundToInt(contentWidth),
            juce::roundToInt(contentHeight),
            ComboBox::getPopupLayoutDimensions().verticalMargin);

        topLevelComponent->addAndMakeVisible(popupMenu.release());
        rawPtr->setBounds(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
    }
}
