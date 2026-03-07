#include "MultiColumnPopupMenu.h"
#include "ComboBox.h"
#include "PopupMenuPositioner.h"

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
        const auto bounds = getLocalBounds().toFloat();
        renderer_.drawBackground(g, bounds);
        const float borderThickness = kBorderThickness_ * scalingFactor_;
        const auto contentBounds = bounds.reduced(borderThickness);
        drawItems(g);
        
        if (columnCount_ > 1)
        {
            const float separatorWidth = getSeparatorWidth();
            const float actualColumnWidth = getActualColumnWidth(contentBounds.getWidth());
            renderer_.drawVerticalSeparators(g, contentBounds, columnCount_, actualColumnWidth, separatorWidth);
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
        columnWidth_ = static_cast<float>(comboBox_.getBaseComponentWidth()) * scalingFactor_;
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

    float MultiColumnPopupMenu::getActualColumnWidth(float contentWidth) const
    {
        if (columnCount_ <= 0)
        {
            return 0.0f;
        }
        const float separatorWidth = getSeparatorWidth();
        return (contentWidth - static_cast<float>(columnCount_ - 1) * separatorWidth) / static_cast<float>(columnCount_);
    }

    juce::Rectangle<float> MultiColumnPopupMenu::getItemBounds(int itemIndex) const
    {
        if (! isValidItemIndex(itemIndex))
        {
            return juce::Rectangle<float>();
        }
        
        const float borderThickness = kBorderThickness_ * scalingFactor_;
        const auto contentBounds = getLocalBounds().toFloat().reduced(borderThickness);
        const float itemHeight = static_cast<float>(kItemHeight_) * scalingFactor_;
        
        if (columnCount_ == 1)
        {
            const float y = contentBounds.getY() + static_cast<float>(itemIndex) * itemHeight;
            return juce::Rectangle<float>(contentBounds.getX(), y, contentBounds.getWidth(), itemHeight);
        }
        
        const int column = itemIndex / itemsPerColumn_;
        const int row = itemIndex % itemsPerColumn_;
        const float separatorWidth = getSeparatorWidth();
        const float actualColumnWidth = getActualColumnWidth(contentBounds.getWidth());
        
        const float x = contentBounds.getX() + static_cast<float>(column) * (actualColumnWidth + separatorWidth);
        const float y = contentBounds.getY() + static_cast<float>(row) * itemHeight;
        
        return juce::Rectangle<float>(x, y, actualColumnWidth, itemHeight);
    }

    int MultiColumnPopupMenu::getItemIndexAt(int x, int y) const
    {
        const float borderThickness = kBorderThickness_ * scalingFactor_;
        const auto contentBounds = getLocalBounds().toFloat().reduced(borderThickness);
        
        if (! contentBounds.contains(static_cast<float>(x), static_cast<float>(y)))
        {
            return -1;
        }
        
        const auto relativeX = static_cast<int>(static_cast<float>(x) - contentBounds.getX());
        const auto relativeY = static_cast<int>(static_cast<float>(y) - contentBounds.getY());
        
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
        
        const float borderThickness = kBorderThickness_ * scalingFactor_;
        const float contentWidth = getLocalBounds().toFloat().reduced(borderThickness).getWidth();
        const float separatorWidth = getSeparatorWidth();
        const float actualColumnWidth = getActualColumnWidth(contentWidth);
        const float columnWithSeparatorWidth = actualColumnWidth + separatorWidth;
        const int estimatedColumn = static_cast<int>(static_cast<float>(x) / columnWithSeparatorWidth);
        
        if (estimatedColumn >= columnCount_)
        {
            return columnCount_ - 1;
        }
        
        const float xInColumn = std::fmod(static_cast<float>(x), columnWithSeparatorWidth);
        const bool isInSeparator = (xInColumn >= actualColumnWidth);
        
        if (isInSeparator && estimatedColumn > 0)
        {
            return estimatedColumn - 1;
        }
        
        return estimatedColumn;
    }
    
    int MultiColumnPopupMenu::getRowFromY(int y) const
    {
        const float itemHeight = static_cast<float>(kItemHeight_) * scalingFactor_;
        return static_cast<int>(static_cast<float>(y) / itemHeight);
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

        const float itemHeight = static_cast<float>(kItemHeight_) * rawPtr->scalingFactor_;
        const float separatorWidth = 1.0f * rawPtr->scalingFactor_;
        const float contentWidth = static_cast<float>(rawPtr->columnCount_) * rawPtr->columnWidth_ + static_cast<float>(rawPtr->columnCount_ - 1) * separatorWidth;
        const float contentHeight = static_cast<float>(rawPtr->itemsPerColumn_) * itemHeight;
        const float borderThickness = kBorderThickness_ * 2.0f * rawPtr->scalingFactor_;

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            comboBox,
            juce::roundToInt(contentWidth + borderThickness),
            juce::roundToInt(contentHeight + borderThickness),
            0);

        topLevelComponent->addAndMakeVisible(popupMenu.release());

        rawPtr->setBounds(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
    }
}
