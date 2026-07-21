#include "HierarchicalPopupMenu.h"

#include "ComboBox.h"
#include "HierarchicalComboBox.h"
#include "PopupMenuModalHelpers.h"
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
    {
        renderer_.setLook(owner.getPopupMenuLook());
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        setOpaque(false);

        measureColumnWidths();

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

        ensureHighlightedPrimaryVisible();
        ensureHighlightedChildVisible();

        if (primaryNeedsScrollbar())
            ensurePrimaryScrollBar();

        if (hasSecondaryColumn() && secondaryNeedsScrollbar())
            ensureSecondaryScrollBar();
    }

    HierarchicalPopupMenu::~HierarchicalPopupMenu() = default;

    void HierarchicalPopupMenu::ensurePrimaryScrollBar()
    {
        if (primaryScrollBar_ != nullptr)
            return;

        const auto& popupLook = owner_.getPopupMenuLook();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(owner_);
        const float thumbInset = static_cast<float>(ScaledDrawing::logicalInsetPixelsFromDesign(
            kThumbInsetBase_,
            uiScale_,
            systemDisplayScale));

        primaryScrollModel_ = std::make_unique<CallbackPopupMenuScrollModel>(
            [this]() {
                return getOpenablePrimaryCount() * juce::roundToInt(getItemHeight());
            },
            [this]() {
                return juce::roundToInt(getMaxViewportContentHeight());
            },
            [this]() {
                return primaryScrollOffset_;
            },
            [this](int y) {
                primaryScrollOffset_ = y;
                clampScrollOffsets();
                repaint();
            });

        primaryScrollBar_ = std::make_unique<PopupMenuCustomScrollBar>(
            *primaryScrollModel_,
            popupLook.scrollbar,
            thumbInset,
            ComboBox::getPopupLayoutDimensions().minThumbHeight,
            uiScale_);
        addAndMakeVisible(*primaryScrollBar_);
    }

    void HierarchicalPopupMenu::ensureSecondaryScrollBar()
    {
        if (secondaryScrollBar_ != nullptr)
            return;

        const auto& popupLook = owner_.getPopupMenuLook();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(owner_);
        const float thumbInset = static_cast<float>(ScaledDrawing::logicalInsetPixelsFromDesign(
            kThumbInsetBase_,
            uiScale_,
            systemDisplayScale));

        secondaryScrollModel_ = std::make_unique<CallbackPopupMenuScrollModel>(
            [this]() {
                return getSecondaryItemCount() * juce::roundToInt(getItemHeight());
            },
            [this]() {
                return juce::roundToInt(getMaxViewportContentHeight());
            },
            [this]() {
                return secondaryScrollOffset_;
            },
            [this](int y) {
                secondaryScrollOffset_ = y;
                clampScrollOffsets();
                repaint();
            });

        secondaryScrollBar_ = std::make_unique<PopupMenuCustomScrollBar>(
            *secondaryScrollModel_,
            popupLook.scrollbar,
            thumbInset,
            ComboBox::getPopupLayoutDimensions().minThumbHeight,
            uiScale_);
        addAndMakeVisible(*secondaryScrollBar_);
    }

    void HierarchicalPopupMenu::measureColumnWidths()
    {
        const float closedWidth = static_cast<float>(owner_.getBaseComponentWidth()) * uiScale_;
        const float textPadding = static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * uiScale_;
        const float chevronReserve = cachedFont_.getHeight() * 0.6f + textPadding;
        const float sidePadding = textPadding + kLabelWidthPadding_ * uiScale_;

        const auto measureLabel = [this](const juce::String& text) {
            return juce::GlyphArrangement::getStringWidth(cachedFont_, text);
        };

        float maxPrimary = closedWidth;
        float maxSecondary = measureLabel("M99-R99") + sidePadding;

        for (int i = 0; i < owner_.getPrimaryItemCount(); ++i)
        {
            const auto& primary = owner_.getPrimaryItem(i);
            if (primary.isSentinel)
                continue;

            float width = measureLabel(primary.label) + sidePadding;
            if (! primary.children.empty())
                width += chevronReserve;

            maxPrimary = juce::jmax(maxPrimary, width);

            for (const auto& child : primary.children)
                maxSecondary = juce::jmax(maxSecondary, measureLabel(child.label) + sidePadding);
        }

        primaryColumnWidth_ = maxPrimary;
        secondaryColumnWidth_ = maxSecondary;
    }

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

    int HierarchicalPopupMenu::getSecondaryItemCount() const
    {
        if (! hasSecondaryColumn() || highlightedPrimaryIndex_ < 0)
            return 0;

        const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
        if (storageIndex < 0)
            return 0;

        return static_cast<int>(owner_.getPrimaryItem(storageIndex).children.size());
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

    float HierarchicalPopupMenu::getMaxViewportContentHeight() const
    {
        return static_cast<float>(ComboBox::getPopupLayoutDimensions().maxScrollHeight) * uiScale_;
    }

    float HierarchicalPopupMenu::getScrollbarThickness() const
    {
        return juce::jmax(1.0f, static_cast<float>(ComboBox::getPopupLayoutDimensions().scrollbarWidth) * uiScale_);
    }

    bool HierarchicalPopupMenu::primaryNeedsScrollbar() const
    {
        return static_cast<float>(getOpenablePrimaryCount()) * getItemHeight() > getMaxViewportContentHeight() + 0.5f;
    }

    bool HierarchicalPopupMenu::secondaryNeedsScrollbar() const
    {
        return static_cast<float>(getSecondaryItemCount()) * getItemHeight() > getMaxViewportContentHeight() + 0.5f;
    }

    float HierarchicalPopupMenu::getPrimaryPanelWidth() const
    {
        const float border = getLayoutBorderThickness();
        const float scrollbar = primaryNeedsScrollbar() ? getScrollbarThickness() : 0.0f;
        return primaryColumnWidth_ + scrollbar + 2.0f * border;
    }

    float HierarchicalPopupMenu::getSecondaryPanelWidth() const
    {
        const float border = getLayoutBorderThickness();
        const float scrollbar = secondaryNeedsScrollbar() ? getScrollbarThickness() : 0.0f;
        return secondaryColumnWidth_ + scrollbar + 2.0f * border;
    }

    float HierarchicalPopupMenu::getPrimaryPanelHeight() const
    {
        const float border = getLayoutBorderThickness();
        const float contentHeight = juce::jmin(static_cast<float>(getOpenablePrimaryCount()) * getItemHeight(),
                                               getMaxViewportContentHeight());
        return contentHeight + 2.0f * border;
    }

    float HierarchicalPopupMenu::getSecondaryPanelHeight() const
    {
        if (! hasSecondaryColumn())
            return 0.0f;

        const float border = getLayoutBorderThickness();
        const float contentHeight = juce::jmin(static_cast<float>(getSecondaryItemCount()) * getItemHeight(),
                                               getMaxViewportContentHeight());
        return contentHeight + 2.0f * border;
    }

    float HierarchicalPopupMenu::getStackHeight() const
    {
        if (! hasSecondaryColumn())
            return getPrimaryPanelHeight();

        return juce::jmax(getPrimaryPanelHeight(), getSecondaryPanelHeight());
    }

    void HierarchicalPopupMenu::clampScrollOffsets()
    {
        const float itemHeight = getItemHeight();
        const int primaryContent = juce::roundToInt(static_cast<float>(getOpenablePrimaryCount()) * itemHeight);
        const int primaryViewport = juce::roundToInt(juce::jmin(static_cast<float>(primaryContent),
                                                                getMaxViewportContentHeight()));
        primaryScrollOffset_ = juce::jlimit(0, juce::jmax(0, primaryContent - primaryViewport), primaryScrollOffset_);

        const int secondaryContent = juce::roundToInt(static_cast<float>(getSecondaryItemCount()) * itemHeight);
        const int secondaryViewport = juce::roundToInt(juce::jmin(static_cast<float>(secondaryContent),
                                                                  getMaxViewportContentHeight()));
        secondaryScrollOffset_ = juce::jlimit(0, juce::jmax(0, secondaryContent - secondaryViewport), secondaryScrollOffset_);
    }

    void HierarchicalPopupMenu::scrollPrimaryBy(int deltaPixels)
    {
        if (! primaryNeedsScrollbar() || deltaPixels == 0)
            return;

        primaryScrollOffset_ += deltaPixels;
        clampScrollOffsets();
        repaint();
        if (primaryScrollBar_ != nullptr)
            primaryScrollBar_->repaint();
    }

    void HierarchicalPopupMenu::scrollSecondaryBy(int deltaPixels)
    {
        if (! secondaryNeedsScrollbar() || deltaPixels == 0)
            return;

        secondaryScrollOffset_ += deltaPixels;
        clampScrollOffsets();
        repaint();
        if (secondaryScrollBar_ != nullptr)
            secondaryScrollBar_->repaint();
    }

    void HierarchicalPopupMenu::ensureHighlightedPrimaryVisible()
    {
        if (highlightedPrimaryIndex_ < 0 || ! primaryNeedsScrollbar())
            return;

        const float itemHeight = getItemHeight();
        const float itemY = static_cast<float>(highlightedPrimaryIndex_) * itemHeight;
        const float viewportHeight = getMaxViewportContentHeight();
        const float viewY = static_cast<float>(primaryScrollOffset_);

        if (itemY < viewY)
            primaryScrollOffset_ = juce::roundToInt(itemY);
        else if (itemY + itemHeight > viewY + viewportHeight)
            primaryScrollOffset_ = juce::roundToInt(itemY + itemHeight - viewportHeight);

        clampScrollOffsets();
    }

    void HierarchicalPopupMenu::ensureHighlightedChildVisible()
    {
        if (highlightedChildIndex_ < 0 || ! secondaryNeedsScrollbar())
            return;

        const float itemHeight = getItemHeight();
        const float itemY = static_cast<float>(highlightedChildIndex_) * itemHeight;
        const float viewportHeight = getMaxViewportContentHeight();
        const float viewY = static_cast<float>(secondaryScrollOffset_);

        if (itemY < viewY)
            secondaryScrollOffset_ = juce::roundToInt(itemY);
        else if (itemY + itemHeight > viewY + viewportHeight)
            secondaryScrollOffset_ = juce::roundToInt(itemY + itemHeight - viewportHeight);

        clampScrollOffsets();
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getPrimaryPanelBounds() const
    {
        const float height = getPrimaryPanelHeight();
        const float y = opensAbove_ ? (getStackHeight() - height) : 0.0f;
        return { 0.0f, y, getPrimaryPanelWidth(), height };
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getSecondaryPanelBounds() const
    {
        if (! hasSecondaryColumn())
            return {};

        const auto primaryPanel = getPrimaryPanelBounds();
        const float border = getLayoutBorderThickness();
        const float height = getSecondaryPanelHeight();
        const float y = opensAbove_ ? (getStackHeight() - height) : 0.0f;

        return {
            primaryPanel.getRight() - border,
            y,
            getSecondaryPanelWidth(),
            height
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

        return panel.reduced(getLayoutBorderThickness());
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getPrimaryItemBounds(int primaryIndex) const
    {
        const auto contentBounds = getPrimaryContentBounds();
        const float itemHeight = getItemHeight();
        return {
            contentBounds.getX(),
            contentBounds.getY() + static_cast<float>(primaryIndex) * itemHeight
                - static_cast<float>(primaryScrollOffset_),
            primaryColumnWidth_,
            itemHeight
        };
    }

    juce::Rectangle<float> HierarchicalPopupMenu::getSecondaryItemBounds(int childIndex) const
    {
        const auto contentBounds = getSecondaryContentBounds();
        if (contentBounds.isEmpty())
            return {};

        if (! juce::isPositiveAndBelow(childIndex, getSecondaryItemCount()))
            return {};

        const float itemHeight = getItemHeight();
        return {
            contentBounds.getX(),
            contentBounds.getY() + static_cast<float>(childIndex) * itemHeight
                - static_cast<float>(secondaryScrollOffset_),
            secondaryColumnWidth_,
            itemHeight
        };
    }

    void HierarchicalPopupMenu::getPreferredContentSize(float& width, float& height) const
    {
        const auto primaryPanel = getPrimaryPanelBounds();
        width = primaryPanel.getWidth();
        height = getStackHeight();

        if (hasSecondaryColumn())
        {
            const auto secondaryPanel = getSecondaryPanelBounds();
            width = secondaryPanel.getRight();
        }
    }

    int HierarchicalPopupMenu::getScaledVerticalMargin() const
    {
        return juce::roundToInt(
            static_cast<float>(ComboBox::getPopupLayoutDimensions().verticalMargin) * uiScale_);
    }

    void HierarchicalPopupMenu::applyPreferredSize()
    {
        float width = 0.0f;
        float height = 0.0f;
        getPreferredContentSize(width, height);

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            owner_,
            juce::roundToInt(width),
            juce::roundToInt(height),
            getScaledVerticalMargin(),
            owner_.getPopupVerticalPlacement());

        opensAbove_ = dimensions.opensAbove;
        getPreferredContentSize(width, height);

        setBounds(dimensions.x,
                  dimensions.y,
                  juce::roundToInt(width),
                  juce::roundToInt(height));

        layoutScrollBars();
        repaint();
    }

    void HierarchicalPopupMenu::layoutScrollBars()
    {
        const float border = getLayoutBorderThickness();
        const float scrollbarWidth = getScrollbarThickness();

        if (primaryNeedsScrollbar())
        {
            ensurePrimaryScrollBar();
            const auto content = getPrimaryContentBounds();
            primaryScrollBar_->setBounds(
                juce::roundToInt(content.getRight() - scrollbarWidth),
                juce::roundToInt(content.getY()),
                juce::roundToInt(scrollbarWidth),
                juce::roundToInt(content.getHeight()));
            primaryScrollBar_->setVisible(true);
        }
        else if (primaryScrollBar_ != nullptr)
        {
            primaryScrollBar_->setVisible(false);
        }

        if (hasSecondaryColumn() && secondaryNeedsScrollbar())
        {
            ensureSecondaryScrollBar();
            const auto content = getSecondaryContentBounds();
            secondaryScrollBar_->setBounds(
                juce::roundToInt(content.getRight() - scrollbarWidth),
                juce::roundToInt(content.getY()),
                juce::roundToInt(scrollbarWidth),
                juce::roundToInt(content.getHeight()));
            secondaryScrollBar_->setVisible(true);
        }
        else if (secondaryScrollBar_ != nullptr)
        {
            secondaryScrollBar_->setVisible(false);
        }

        juce::ignoreUnused(border);
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

    void HierarchicalPopupMenu::drawAlignedPanelBorders(juce::Graphics& g,
                                                        juce::Rectangle<float> primaryPanel,
                                                        juce::Rectangle<float> secondaryPanel,
                                                        float thickness) const
    {
        // Shared edge: omit primary right / secondary left, then stroke the join once.
        drawPanelBorderEdges(g, primaryPanel, thickness, true, true, false, true);
        drawPanelBorderEdges(g, secondaryPanel, thickness, false, true, true, true);

        const float sharedX = primaryPanel.getRight() - thickness;
        const float joinTop = juce::jmax(primaryPanel.getY(), secondaryPanel.getY());
        const float joinBottom = juce::jmin(primaryPanel.getBottom(), secondaryPanel.getBottom());
        if (joinBottom > joinTop)
            g.fillRect(sharedX, joinTop, thickness, joinBottom - joinTop);

        // Cap any protruding vertical segment at the shared edge outside the secondary.
        if (secondaryPanel.getY() > primaryPanel.getY())
            g.fillRect(sharedX, primaryPanel.getY(), thickness, secondaryPanel.getY() - primaryPanel.getY());

        if (secondaryPanel.getBottom() < primaryPanel.getBottom())
            g.fillRect(sharedX,
                       secondaryPanel.getBottom(),
                       thickness,
                       primaryPanel.getBottom() - secondaryPanel.getBottom());

        if (primaryPanel.getY() > secondaryPanel.getY())
            g.fillRect(sharedX, secondaryPanel.getY(), thickness, primaryPanel.getY() - secondaryPanel.getY());

        if (primaryPanel.getBottom() < secondaryPanel.getBottom())
            g.fillRect(sharedX,
                       primaryPanel.getBottom(),
                       thickness,
                       secondaryPanel.getBottom() - primaryPanel.getBottom());

        // Close secondary left edge above/below shared join (already covered by fillRect join when aligned).
        // Re-draw secondary left only where it does not overlap primary height.
        if (secondaryPanel.getY() < primaryPanel.getY())
            g.fillRect(secondaryPanel.getX(),
                       secondaryPanel.getY(),
                       thickness,
                       primaryPanel.getY() - secondaryPanel.getY());

        if (secondaryPanel.getBottom() > primaryPanel.getBottom())
            g.fillRect(secondaryPanel.getX(),
                       primaryPanel.getBottom(),
                       thickness,
                       secondaryPanel.getBottom() - primaryPanel.getBottom());
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
        const auto primaryContent = getPrimaryContentBounds();
        const auto popupLook = owner_.getPopupMenuLook();

        renderer_.drawBackground(g, primaryPanel);

        {
            juce::Graphics::ScopedSaveState clipState(g);
            g.reduceClipRegion(primaryContent.toNearestInt());

            for (int primaryIndex = 0; primaryIndex < getOpenablePrimaryCount(); ++primaryIndex)
            {
                const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, primaryIndex);
                if (storageIndex < 0)
                    continue;

                const auto& primary = owner_.getPrimaryItem(storageIndex);
                const auto itemBounds = getPrimaryItemBounds(primaryIndex);
                if (! itemBounds.intersects(primaryContent))
                    continue;

                const bool isHighlighted = primaryIndex == highlightedPrimaryIndex_;
                renderer_.drawLabelItem(g, primary.label, itemBounds, isHighlighted, true, cachedFont_);

                if (! primary.children.empty())
                    renderer_.drawSubMenuChevron(g, itemBounds, cachedFont_);
            }
        }

        if (hasSecondaryColumn())
        {
            const auto secondaryPanel = getSecondaryPanelBounds();
            const auto secondaryContent = getSecondaryContentBounds();
            const auto storageIndex = primaryStorageIndexForOpenableIndex(owner_, highlightedPrimaryIndex_);
            const auto& primary = owner_.getPrimaryItem(storageIndex);

            renderer_.drawBackground(g, secondaryPanel);

            {
                juce::Graphics::ScopedSaveState clipState(g);
                g.reduceClipRegion(secondaryContent.toNearestInt());

                for (size_t childIndex = 0; childIndex < primary.children.size(); ++childIndex)
                {
                    const auto itemBounds = getSecondaryItemBounds(static_cast<int>(childIndex));
                    if (! itemBounds.intersects(secondaryContent))
                        continue;

                    const bool isHighlighted = highlightedChildIndex_ == static_cast<int>(childIndex);
                    renderer_.drawLabelItem(g,
                                            primary.children[childIndex].label,
                                            itemBounds,
                                            isHighlighted,
                                            true,
                                            cachedFont_);
                }
            }

            g.setColour(popupLook.border);
            drawAlignedPanelBorders(g, primaryPanel, secondaryPanel, getLayoutBorderThickness());
        }
        else
        {
            g.setColour(popupLook.border);
            drawPanelBorderEdges(g, primaryPanel, getLayoutBorderThickness(), true, true, true, true);
        }
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
        closePopup();
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
            juce::roundToInt(contentWidth),
            juce::roundToInt(contentHeight),
            rawPtr->getScaledVerticalMargin(),
            owner.getPopupVerticalPlacement());

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
