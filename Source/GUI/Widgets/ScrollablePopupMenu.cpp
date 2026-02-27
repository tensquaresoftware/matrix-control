#include "ScrollablePopupMenu.h"
#include "ComboBox.h"
#include "PopupMenuPositioner.h"

#include "GUI/Skins/Skin.h"

namespace tss
{
    class ScrollablePopupMenu::ScrollableViewportLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        explicit ScrollableViewportLookAndFeel(const juce::Colour& scrollbarColour)
            : scrollbarColour_(scrollbarColour)
        {
        }

        void drawScrollbar(juce::Graphics& g, juce::ScrollBar& /*scrollbar*/, int x, int y, int width, int height,
                          bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                          bool isMouseOver, bool isMouseDown) override
        {
            g.fillAll(juce::Colours::transparentBlack);
            
            juce::Rectangle<int> thumbBounds;
            if (isScrollbarVertical)
            {
                thumbBounds = juce::Rectangle<int>(x, thumbStartPosition, width, thumbSize);
            }
            else
            {
                thumbBounds = juce::Rectangle<int>(thumbStartPosition, y, thumbSize, height);
            }
            
            auto brighterColour = scrollbarColour_;
            if (isMouseOver || isMouseDown)
            {
                brighterColour = scrollbarColour_.brighter(0.2f);
            }
            
            g.setColour(brighterColour);
            g.fillRect(thumbBounds.reduced(2));
        }

    private:
        juce::Colour scrollbarColour_;
    };

    class ScrollablePopupMenu::ScrollableContentComponent : public juce::Component
    {
    public:
        explicit ScrollableContentComponent(ScrollablePopupMenu& popupMenu)
            : popupMenu_(popupMenu)
        {
        }

        void paint(juce::Graphics& g) override
        {
            popupMenu_.drawItems(g);
        }

        void mouseMove(const juce::MouseEvent& e) override
        {
            const auto itemIndex = popupMenu_.getItemIndexAt(e.getPosition().x, e.getPosition().y);
            popupMenu_.updateHighlightedItem(itemIndex);
        }

        void mouseExit(const juce::MouseEvent&) override
        {
            popupMenu_.updateHighlightedItem(-1);
        }

        void mouseUp(const juce::MouseEvent& e) override
        {
            const auto itemIndex = popupMenu_.getItemIndexAt(e.getPosition().x, e.getPosition().y);
            if (itemIndex >= 0)
            {
                popupMenu_.selectItem(itemIndex);
            }
        }

    private:
        ScrollablePopupMenu& popupMenu_;
    };

    ScrollablePopupMenu::ScrollablePopupMenu(ComboBox& comboBox)
        : PopupMenuBase(comboBox, true)
    {
        const auto numItems = comboBox_.getNumItems();
        columnWidth_ = comboBox_.getBounds().getWidth();
        scrollableContentHeight_ = numItems * kItemHeight_;
        
        setupScrollableContent();
        
        const auto selectedIndex = comboBox_.getSelectedItemIndex();
        if (isValidItemIndex(selectedIndex))
        {
            highlightedItemIndex_ = selectedIndex;
        }
        else if (numItems > 0)
        {
            highlightedItemIndex_ = 0;
        }
        
        if (viewport_ != nullptr && highlightedItemIndex_ >= 0)
        {
            const auto itemY = highlightedItemIndex_ * kItemHeight_;
            viewport_->setViewPosition(0, itemY);
        }
    }

    ScrollablePopupMenu::~ScrollablePopupMenu()
    {
        if (viewport_ != nullptr)
        {
            viewport_->setLookAndFeel(nullptr);
        }
    }

    void ScrollablePopupMenu::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
        {
            return;
        }

        g.fillAll(skin_->getPopupMenuBackgroundColour(isButtonLike_));
        
        const auto bounds = getLocalBounds();
        const auto borderThickness = static_cast<int>(kBorderThickness_);
        
        renderer_.drawBackground(g, bounds.reduced(borderThickness));
        renderer_.drawBorder(g, bounds);
    }

    void ScrollablePopupMenu::resized()
    {
        if (viewport_ != nullptr)
        {
            const auto borderThickness = static_cast<int>(kBorderThickness_);
            const auto contentBounds = getLocalBounds().reduced(borderThickness);
            viewport_->setBounds(contentBounds);
        }
    }

    void ScrollablePopupMenu::setupScrollableContent()
    {
        contentComponent_ = std::make_unique<ScrollableContentComponent>(*this);
        contentComponent_->setSize(columnWidth_, scrollableContentHeight_);
        
        viewport_ = std::make_unique<juce::Viewport>();
        viewport_->setViewedComponent(contentComponent_.get(), false);
        viewport_->setScrollBarsShown(true, false);
        viewport_->setScrollBarThickness(8);
        
        if (auto* currentSkin = skin_)
        {
            const auto scrollbarColour = currentSkin->getPopupMenuScrollbarColour(isButtonLike_);
            scrollbarLookAndFeel_ = std::make_unique<ScrollableViewportLookAndFeel>(scrollbarColour);
            viewport_->setLookAndFeel(scrollbarLookAndFeel_.get());
        }
        
        addAndMakeVisible(*viewport_);
    }

    juce::Rectangle<int> ScrollablePopupMenu::getItemBounds(int itemIndex) const
    {
        if (! isValidItemIndex(itemIndex))
        {
            return juce::Rectangle<int>();
        }
        
        if (contentComponent_ != nullptr)
        {
            const auto y = itemIndex * kItemHeight_;
            return juce::Rectangle<int>(0, y, contentComponent_->getWidth(), kItemHeight_);
        }
        
        return juce::Rectangle<int>();
    }

    int ScrollablePopupMenu::getItemIndexAt(int x, int y) const
    {
        if (contentComponent_ != nullptr)
        {
            if (contentComponent_->getBounds().contains(x, y))
            {
                const auto row = y / kItemHeight_;
                if (row >= 0 && row < comboBox_.getNumItems())
                {
                    return row;
                }
            }
        }
        
        return -1;
    }

    void ScrollablePopupMenu::drawItems(juce::Graphics& g)
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

    void ScrollablePopupMenu::handleKeyboardNavigation(const juce::KeyPress& key)
    {
        if (key.getKeyCode() == juce::KeyPress::upKey)
        {
            navigateUp();
        }
        else if (key.getKeyCode() == juce::KeyPress::downKey)
        {
            navigateDown();
        }
    }

    void ScrollablePopupMenu::scrollToHighlightedItem()
    {
        if (viewport_ == nullptr || contentComponent_ == nullptr || highlightedItemIndex_ < 0)
        {
            return;
        }

        const auto itemY = highlightedItemIndex_ * kItemHeight_;
        const auto viewportY = viewport_->getViewPositionY();
        const auto viewportHeight = viewport_->getHeight();

        const auto isItemVisible = (itemY >= viewportY && itemY + kItemHeight_ <= viewportY + viewportHeight);
        if (! isItemVisible)
        {
            const auto centeredY = juce::jmax(0, itemY - viewportHeight / 2);
            viewport_->setViewPosition(0, centeredY);
        }
    }

    void ScrollablePopupMenu::navigateAndScroll(int delta)
    {
        const auto newIndex = highlightedItemIndex_ + delta;
        const auto numItems = static_cast<const juce::ComboBox&>(comboBox_).getNumItems();

        if (newIndex >= 0 && newIndex < numItems)
        {
            updateHighlightedItem(newIndex);
            scrollToHighlightedItem();
        }
    }

    void ScrollablePopupMenu::navigateUp()
    {
        if (highlightedItemIndex_ < 0)
        {
            updateHighlightedItem(0);
            return;
        }

        navigateAndScroll(-1);
    }

    void ScrollablePopupMenu::navigateDown()
    {
        if (highlightedItemIndex_ < 0)
        {
            updateHighlightedItem(0);
            return;
        }

        navigateAndScroll(1);
    }

    void ScrollablePopupMenu::show(ComboBox& comboBox)
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

        auto popupMenu = std::make_unique<ScrollablePopupMenu>(comboBox);
        auto* rawPtr = popupMenu.get();

        const auto contentWidth = rawPtr->columnWidth_;
        const auto contentHeight = juce::jmin(rawPtr->scrollableContentHeight_, kMaxScrollableHeight_);
        const auto borderThickness = static_cast<int>(kBorderThickness_ * 2.0f);

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            comboBox, contentWidth, contentHeight, borderThickness);

        topLevelComponent->addAndMakeVisible(popupMenu.release());

        rawPtr->setBounds(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
        rawPtr->resized();
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
    }
}
