// Extracted from HierarchicalPopupMenu.cpp for modular maintenance.
// Geometry, scrollbars, and preferred-size layout for the hierarchical popup.

#include "HierarchicalPopupMenu.h"

#include "ComboBox.h"
#include "HierarchicalComboBox.h"
#include "PopupMenuPositioner.h"

namespace TSS
{
    void HierarchicalPopupMenu::ensurePrimaryScrollBar()
    {
        if (primaryScrollBar_ != nullptr)
            return;

        const auto& popupLook = owner_.getPopupMenuLook();
        const float thumbInset = PopupMenuCustomScrollBar::thumbInsetLogicalPixels(
            kThumbInsetBase_, uiScale_);

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
        const float thumbInset = PopupMenuCustomScrollBar::thumbInsetLogicalPixels(
            kThumbInsetBase_, uiScale_);

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
            if (! primary.isSelectable())
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
            PopupMenuPositioner::DimensionsArgs{
                .popupWidth = juce::roundToInt(width),
                .popupHeight = juce::roundToInt(height),
                .verticalMargin = getScaledVerticalMargin(),
                .placement = owner_.getPopupVerticalPlacement()});

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
    }
}
