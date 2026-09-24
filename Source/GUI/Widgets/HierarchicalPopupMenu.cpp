#include "HierarchicalPopupMenu.h"

#include "ComboBox.h"
#include "HierarchicalComboBox.h"
#include "PopupMenuModalHelpers.h"
#include "PopupMenuPositioner.h"

namespace TSS
{
    int HierarchicalPopupMenu::countOpenablePrimaries(const HierarchicalComboBox& owner)
    {
        int count = 0;
        for (int i = 0; i < owner.getPrimaryItemCount(); ++i)
        {
            if (owner.getPrimaryItem(i).isSelectable())
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
            if (! primary.isSelectable())
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
            if (! owner.getPrimaryItem(i).isSelectable())
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
    {
        renderer_.setLook(owner.getPopupMenuLook());
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        setOpaque(false);

        measureColumnWidths();

        highlightedPrimaryIndex_ = resolveInitialHighlightedPrimaryIndex();
        highlightedChildIndex_ = resolveInitialHighlightedChildIndex();

        ensureHighlightedPrimaryVisible();
        ensureHighlightedChildVisible();

        if (primaryNeedsScrollbar())
            ensurePrimaryScrollBar();

        if (hasSecondaryColumn() && secondaryNeedsScrollbar())
            ensureSecondaryScrollBar();
    }

    HierarchicalPopupMenu::~HierarchicalPopupMenu() = default;

    int HierarchicalPopupMenu::resolveInitialHighlightedPrimaryIndex() const
    {
        const int rowCount = countOpenablePrimaries(owner_);
        int rowIndex = openableIndexForPrimaryId(owner_, owner_.getSelectedPrimaryId());
        if (rowIndex < 0)
            rowIndex = 0;

        return rowIndex < rowCount ? rowIndex : -1;
    }

    int HierarchicalPopupMenu::resolveInitialHighlightedChildIndex() const
    {
        if (highlightedPrimaryIndex_ < 0)
            return -1;

        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        if (storageIndex < 0)
            return -1;

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        for (size_t i = 0; i < primary.children.size(); ++i)
        {
            if (primary.children[i].id == owner_.getSelectedChildId())
                return static_cast<int>(i);
        }

        return -1;
    }

    int HierarchicalPopupMenu::getPrimaryIndexAt(int x, int y) const
    {
        const auto contentBounds = getPrimaryContentBounds();
        if (! contentBounds.contains(static_cast<float>(x), static_cast<float>(y)))
            return -1;

        if (primaryNeedsScrollbar())
        {
            const float scrollbarLeft = contentBounds.getRight() - getScrollbarThickness();
            if (static_cast<float>(x) >= scrollbarLeft)
                return -1;
        }

        const float localY = static_cast<float>(y) - contentBounds.getY() + static_cast<float>(primaryScrollOffset_);
        const int row = static_cast<int>(localY / getItemHeight());
        if (row < 0 || row >= getOpenablePrimaryCount())
            return -1;

        return row;
    }

    int HierarchicalPopupMenu::getChildIndexAt(int x, int y) const
    {
        if (! hasSecondaryColumn())
            return -1;

        const auto contentBounds = getSecondaryContentBounds();
        if (! contentBounds.contains(static_cast<float>(x), static_cast<float>(y)))
            return -1;

        // Ignore clicks on the scrollbar track.
        if (secondaryNeedsScrollbar())
        {
            const float scrollbarLeft = contentBounds.getRight() - getScrollbarThickness();
            if (static_cast<float>(x) >= scrollbarLeft)
                return -1;
        }

        const float localY = static_cast<float>(y) - contentBounds.getY() + static_cast<float>(secondaryScrollOffset_);
        const int row = static_cast<int>(localY / getItemHeight());
        if (row < 0 || row >= getSecondaryItemCount())
            return -1;

        return row;
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
        if (primaryIndex < 0)
            return;

        if (primaryIndex != highlightedPrimaryIndex_)
        {
            highlightedPrimaryIndex_ = primaryIndex;
            highlightedChildIndex_ = -1;
            secondaryScrollOffset_ = 0;
            clampScrollOffsets();
            applyPreferredSize();
            return;
        }

        // Same primary row: clear N2 row highlight when the pointer leaves N2.
        if (highlightedChildIndex_ >= 0)
        {
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

        // Commit after teardown so onChange rebuild/clear cannot run under a live popup.
        closePopupWithSelection(primary.id, 0);
    }

    void HierarchicalPopupMenu::selectChild(int primaryIndex, int childIndex)
    {
        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, primaryIndex);
        if (storageIndex < 0)
            return;

        const auto& primary = owner_.getPrimaryItem(storageIndex);
        if (! juce::isPositiveAndBelow(childIndex, static_cast<int>(primary.children.size())))
            return;

        const int primaryId = primary.id;
        const int childId = primary.children[static_cast<size_t>(childIndex)].id;
        closePopupWithSelection(primaryId, childId);
    }

    void HierarchicalPopupMenu::closePopup()
    {
        PopupMenuModalHelpers::dismissAndDelete(*this, owner_);
    }

    void HierarchicalPopupMenu::closePopupWithSelection(int primaryId, int childId)
    {
        // Keep commit in this friend member (not a lambda) so private API stays accessible.
        auto& owner = owner_;
        exitModalState(0);
        owner.notifyPopupClosed();

        if (auto* parent = getParentComponent())
            parent->removeChildComponent(this);

        owner.commitSelectionFromPopup(primaryId, childId);
        delete this;
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

    void HierarchicalPopupMenu::resized()
    {
        layoutScrollBars();
    }

    void HierarchicalPopupMenu::mouseMove(const juce::MouseEvent& e)
    {
        updateHighlightFromPosition(e.getPosition().x, e.getPosition().y);
    }

    void HierarchicalPopupMenu::mouseUp(const juce::MouseEvent& e)
    {
        const auto childIndex = getChildIndexAt(e.getPosition().x, e.getPosition().y);
        if (childIndex >= 0)
        {
            selectChild(highlightedPrimaryIndex_, childIndex);
            return;
        }

        // Ignore clicks on the primary scrollbar.
        const auto primaryContent = getPrimaryContentBounds();
        if (primaryNeedsScrollbar()
            && primaryContent.contains(static_cast<float>(e.x), static_cast<float>(e.y))
            && static_cast<float>(e.x) >= primaryContent.getRight() - getScrollbarThickness())
        {
            return;
        }

        const auto primaryIndex = getPrimaryIndexAt(e.getPosition().x, e.getPosition().y);
        if (primaryIndex >= 0)
            selectPrimaryLeaf(primaryIndex);
    }

    void HierarchicalPopupMenu::mouseExit(const juce::MouseEvent&)
    {
    }

    void HierarchicalPopupMenu::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
    {
        const int delta = juce::roundToInt(-wheel.deltaY * kWheelScrollFactorContent_);
        if (delta == 0)
            return;

        if (hasSecondaryColumn()
            && getSecondaryPanelBounds().contains(static_cast<float>(e.x), static_cast<float>(e.y)))
        {
            scrollSecondaryBy(delta);
            return;
        }

        if (getPrimaryPanelBounds().contains(static_cast<float>(e.x), static_cast<float>(e.y)))
            scrollPrimaryBy(delta);
    }

    void HierarchicalPopupMenu::inputAttemptWhenModal()
    {
        PopupMenuModalHelpers::dismissFromOutsideClick(*this, owner_);
    }

    bool HierarchicalPopupMenu::keyPressed(const juce::KeyPress& key)
    {
        return PopupMenuModalHelpers::handleEscapeKey(key, *this, owner_);
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
            PopupMenuPositioner::DimensionsArgs{
                .popupWidth = juce::roundToInt(contentWidth),
                .popupHeight = juce::roundToInt(contentHeight),
                .verticalMargin = rawPtr->getScaledVerticalMargin(),
                .placement = owner.getPopupVerticalPlacement()});

        rawPtr->opensAbove_ = dimensions.opensAbove;

        // Recompute panel Y offsets now that opensAbove_ is known.
        rawPtr->getPreferredContentSize(contentWidth, contentHeight);

        topLevelComponent->addAndMakeVisible(popupMenu.release());
        rawPtr->setBounds(dimensions.x,
                          dimensions.y,
                          juce::roundToInt(contentWidth),
                          juce::roundToInt(contentHeight));
        rawPtr->layoutScrollBars();
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
    }
}
