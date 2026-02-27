#include "MultiColumnPopupMenu.h"
#include "ComboBox.h"
#include "PopupMenuPositioner.h"

#include "GUI/Skins/Skin.h"

namespace tss
{
    MultiColumnPopupMenu::MultiColumnPopupMenu(ComboBox& comboBox)
        : PopupMenuBase(comboBox, false)
    {
        calculateLayout();
        
        const auto selectedIndex = comboBox_.getSelectedItemIndex();
        if (isValidItemIndex(selectedIndex))
        {
            highlightedItemIndex_ = selectedIndex;
        }
        else if (comboBox_.getNumItems() > 0)
        {
            highlightedItemIndex_ = 0;
        }
    }

    void MultiColumnPopupMenu::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
        {
            return;
        }

        g.fillAll(skin_->getPopupMenuBackgroundColour(isButtonLike_));
        
        const auto bounds = getLocalBounds();
        const auto borderThickness = static_cast<int>(kBorderThickness_);
        const auto contentBounds = bounds.reduced(borderThickness);
        
        renderer_.drawBackground(g, contentBounds);
        drawItems(g);
        
        if (columnCount_ > 1)
        {
            renderer_.drawVerticalSeparators(g, contentBounds, columnCount_, columnWidth_, kSeparatorWidth_);
        }
        
        renderer_.drawBorder(g, bounds);
    }

    void MultiColumnPopupMenu::mouseMove(const juce::MouseEvent& e)
    {
        const auto itemIndex = getItemIndexAt(e.getPosition().x, e.getPosition().y);
        updateHighlightedItem(itemIndex);
    }

    void MultiColumnPopupMenu::mouseUp(const juce::MouseEvent& e)
    {
        const auto itemIndex = getItemIndexAt(e.getPosition().x, e.getPosition().y);
        if (itemIndex >= 0)
        {
            selectItem(itemIndex);
        }
    }

    void MultiColumnPopupMenu::calculateLayout()
    {
        const auto numItems = comboBox_.getNumItems();
        if (numItems == 0)
        {
            return;
        }
        
        columnCount_ = calculateColumnCount(numItems);
        itemsPerColumn_ = calculateItemsPerColumn(numItems, columnCount_);
        columnWidth_ = comboBox_.getBounds().getWidth();
    }

    int MultiColumnPopupMenu::calculateColumnCount(int totalItems) const
    {
        if (totalItems <= kColumnThreshold_)
        {
            return 1;
        }
        
        return (totalItems + kColumnThreshold_ - 1) / kColumnThreshold_;
    }

    int MultiColumnPopupMenu::calculateItemsPerColumn(int totalItems, int numColumns) const
    {
        return (totalItems + numColumns - 1) / numColumns;
    }

    juce::Rectangle<int> MultiColumnPopupMenu::getItemBounds(int itemIndex) const
    {
        if (! isValidItemIndex(itemIndex))
        {
            return juce::Rectangle<int>();
        }
        
        const auto borderThickness = kBorderThickness_;
        const auto contentBounds = getLocalBounds().reduced(static_cast<int>(borderThickness));
        
        if (columnCount_ == 1)
        {
            const auto y = contentBounds.getY() + itemIndex * kItemHeight_;
            return juce::Rectangle<int>(contentBounds.getX(), y, contentBounds.getWidth(), kItemHeight_);
        }
        
        const auto column = itemIndex / itemsPerColumn_;
        const auto row = itemIndex % itemsPerColumn_;
        
        const auto x = contentBounds.getX() + column * columnWidth_ + column * kSeparatorWidth_;
        const auto y = contentBounds.getY() + row * kItemHeight_;
        
        return juce::Rectangle<int>(x, y, columnWidth_, kItemHeight_);
    }

    int MultiColumnPopupMenu::getItemIndexAt(int x, int y) const
    {
        const auto contentBounds = getLocalBounds().reduced(static_cast<int>(kBorderThickness_));
        
        if (! contentBounds.contains(x, y))
        {
            return -1;
        }
        
        const auto relativeX = x - contentBounds.getX();
        const auto relativeY = y - contentBounds.getY();
        
        const auto column = getColumnFromX(relativeX);
        const auto row = getRowFromY(relativeY);
        
        if (column < 0 || column >= columnCount_ || row < 0 || row >= itemsPerColumn_)
        {
            return -1;
        }
        
        const auto itemIndex = getItemIndexFromColumnAndRow(column, row);
        
        return isValidItemIndex(itemIndex) ? itemIndex : -1;
    }

    int MultiColumnPopupMenu::getColumnFromX(int x) const
    {
        if (columnCount_ == 1)
        {
            return 0;
        }

        const auto columnWithSeparatorWidth = columnWidth_ + kSeparatorWidth_;
        const auto estimatedColumn = x / columnWithSeparatorWidth;

        if (estimatedColumn >= columnCount_)
        {
            return columnCount_ - 1;
        }

        const auto xInColumn = x % columnWithSeparatorWidth;
        const auto isInSeparator = (xInColumn >= columnWidth_);

        if (isInSeparator && estimatedColumn > 0)
        {
            return estimatedColumn - 1;
        }

        return estimatedColumn;
    }

    int MultiColumnPopupMenu::getRowFromY(int y) const
    {
        return y / kItemHeight_;
    }

    int MultiColumnPopupMenu::getItemIndexFromColumnAndRow(int column, int row) const
    {
        if (columnCount_ == 1)
        {
            return row;
        }
        
        return column * itemsPerColumn_ + row;
    }

    void MultiColumnPopupMenu::drawItems(juce::Graphics& g)
    {
        const auto numItems = comboBox_.getNumItems();
        
        for (int i = 0; i < numItems; ++i)
        {
            const auto itemBounds = getItemBounds(i);
            if (! itemBounds.isEmpty())
            {
                renderer_.drawItem(g, comboBox_, i, itemBounds, highlightedItemIndex_, cachedFont_);
            }
        }
    }

    void MultiColumnPopupMenu::handleKeyboardNavigation(const juce::KeyPress& key)
    {
        if (key.getKeyCode() == juce::KeyPress::upKey)
        {
            navigateUp();
        }
        else if (key.getKeyCode() == juce::KeyPress::downKey)
        {
            navigateDown();
        }
        else if (key.getKeyCode() == juce::KeyPress::leftKey)
        {
            navigateLeft();
        }
        else if (key.getKeyCode() == juce::KeyPress::rightKey)
        {
            navigateRight();
        }
    }

    bool MultiColumnPopupMenu::tryNavigateToItemIndex(int itemIndex)
    {
        if (isValidItemIndex(itemIndex))
        {
            updateHighlightedItem(itemIndex);
            return true;
        }
        return false;
    }

    void MultiColumnPopupMenu::navigateInSingleColumn(int delta)
    {
        const auto newIndex = highlightedItemIndex_ + delta;
        tryNavigateToItemIndex(newIndex);
    }

    void MultiColumnPopupMenu::navigateInMultiColumn(int columnDelta, int rowDelta)
    {
        const auto column = highlightedItemIndex_ / itemsPerColumn_;
        const auto row = highlightedItemIndex_ % itemsPerColumn_;
        const auto newColumn = column + columnDelta;
        const auto newRow = row + rowDelta;

        if (newColumn >= 0 && newColumn < columnCount_ && newRow >= 0 && newRow < itemsPerColumn_)
        {
            const auto newIndex = newColumn * itemsPerColumn_ + newRow;
            tryNavigateToItemIndex(newIndex);
        }
    }

    void MultiColumnPopupMenu::navigateUp()
    {
        if (highlightedItemIndex_ < 0)
        {
            tryNavigateToItemIndex(0);
            return;
        }

        if (columnCount_ == 1)
        {
            navigateInSingleColumn(-1);
        }
        else
        {
            navigateInMultiColumn(0, -1);
        }
    }

    void MultiColumnPopupMenu::navigateDown()
    {
        if (highlightedItemIndex_ < 0)
        {
            tryNavigateToItemIndex(0);
            return;
        }

        if (columnCount_ == 1)
        {
            navigateInSingleColumn(1);
        }
        else
        {
            navigateInMultiColumn(0, 1);
        }
    }

    void MultiColumnPopupMenu::navigateLeft()
    {
        if (columnCount_ <= 1 || highlightedItemIndex_ < 0)
        {
            return;
        }

        navigateInMultiColumn(-1, 0);
    }

    void MultiColumnPopupMenu::navigateRight()
    {
        if (columnCount_ <= 1 || highlightedItemIndex_ < 0)
        {
            return;
        }

        navigateInMultiColumn(1, 0);
    }

    namespace
    {
        int calculateContentWidth(int columnCount, int columnWidth)
        {
            constexpr int kSeparatorWidth = 1;
            return columnCount * columnWidth + (columnCount - 1) * kSeparatorWidth;
        }
    }

    void MultiColumnPopupMenu::show(ComboBox& comboBox)
    {
        if (comboBox.getNumItems() == 0)
        {
            return;
        }

        auto* topLevelComponent = comboBox.getTopLevelComponent();
        if (topLevelComponent == nullptr)
        {
            return;
        }

        auto popupMenu = std::make_unique<MultiColumnPopupMenu>(comboBox);
        auto* rawPtr = popupMenu.get();

        const auto contentWidth = calculateContentWidth(rawPtr->columnCount_, rawPtr->columnWidth_);
        const auto contentHeight = rawPtr->itemsPerColumn_ * kItemHeight_;
        const auto borderThickness = static_cast<int>(kBorderThickness_ * 2.0f);

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            comboBox, contentWidth, contentHeight, borderThickness);

        topLevelComponent->addAndMakeVisible(popupMenu.release());

        rawPtr->setBounds(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
    }
}
