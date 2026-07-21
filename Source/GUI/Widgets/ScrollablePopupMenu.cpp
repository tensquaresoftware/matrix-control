#include "ScrollablePopupMenu.h"
#include "ComboBox.h"
#include "PopupMenuPositioner.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ColourChart.h"

namespace TSS
{
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
                popupMenu_.selectItem(itemIndex);
        }

        void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override
        {
            const int delta = juce::roundToInt(-wheel.deltaY * ScrollablePopupMenu::kWheelScrollFactorContent_);
            if (delta != 0)
                popupMenu_.scrollViewportBy(delta);
        }

    private:
        ScrollablePopupMenu& popupMenu_;
    };

    ScrollablePopupMenu::ScrollablePopupMenu(ComboBox& comboBox)
        : PopupMenuBase(comboBox, true)
    {
        const auto numItems = comboBox_.getNumItems();
        columnWidth_ = static_cast<float>(comboBox_.getBaseComponentWidth()) * uiScale_;
        const auto& popupLayout = ComboBox::getPopupLayoutDimensions();
        scrollableContentHeight_ = juce::roundToInt(static_cast<float>(numItems * popupLayout.itemHeight) * uiScale_);

        const float maxScrollableHeight = static_cast<float>(popupLayout.maxScrollHeight) * uiScale_;
        scrollbarNeeded_ = (scrollableContentHeight_ > static_cast<int>(maxScrollableHeight));

        setupScrollableContent();

        const auto selectedIndex = comboBox_.getSelectedItemIndex();
        if (isValidItemIndex(selectedIndex))
            highlightedItemIndex_ = selectedIndex;
        else if (numItems > 0)
            highlightedItemIndex_ = 0;

        if (viewport_ != nullptr && highlightedItemIndex_ >= 0)
        {
            const auto itemY = juce::roundToInt(static_cast<float>(highlightedItemIndex_ * popupLayout.itemHeight) * uiScale_);
            viewport_->setViewPosition(0, itemY);
            repaintScrollBar();
        }
    }

    ScrollablePopupMenu::~ScrollablePopupMenu() = default;

    void ScrollablePopupMenu::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            getBorderThicknessDesign(),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kFloor);
        const auto contentBounds = bounds.reduced(borderThickness);
        renderer_.drawBackground(g, contentBounds);
        renderer_.drawBorder(g, bounds, systemDisplayScale);
    }

    void ScrollablePopupMenu::resized()
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            getBorderThicknessDesign(),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kFloor);
        const int insetPx = juce::roundToInt(borderThickness);
        auto inner = getLocalBounds().reduced(insetPx);
        if (viewport_ == nullptr)
            return;

        if (scrollbarNeeded_ && customScrollBar_ != nullptr)
        {
            const int scrollbarThicknessPx = juce::jmax(1, juce::roundToInt(
                static_cast<float>(ComboBox::getPopupLayoutDimensions().scrollbarWidth) * uiScale_));
            viewport_->setBounds(inner.removeFromLeft(inner.getWidth() - scrollbarThicknessPx));
            customScrollBar_->setBounds(inner);
        }
        else
        {
            viewport_->setBounds(inner);
        }
    }

    void ScrollablePopupMenu::setupScrollableContent()
    {
        contentComponent_ = std::make_unique<ScrollableContentComponent>(*this);
        contentComponent_->setSize(juce::roundToInt(columnWidth_), scrollableContentHeight_);

        viewport_ = std::make_unique<juce::Viewport>();
        viewport_->setViewedComponent(contentComponent_.get(), false);
        viewport_->setScrollBarsShown(false, false);

        addAndMakeVisible(*viewport_);

        if (scrollbarNeeded_)
        {
            const auto& popupLook = comboBox_.getPopupMenuLook();
            const juce::Colour scrollbarColour = isButtonLike_
                ? popupLook.scrollbarButtonLike
                : popupLook.scrollbar;
            const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
            const float thumbInset = static_cast<float>(ScaledDrawing::logicalInsetPixelsFromDesign(
                kThumbInsetBase_,
                uiScale_,
                systemDisplayScale));

            scrollModel_ = std::make_unique<ViewportPopupMenuScrollModel>(*viewport_);
            customScrollBar_ = std::make_unique<PopupMenuCustomScrollBar>(
                *scrollModel_,
                scrollbarColour,
                thumbInset,
                ComboBox::getPopupLayoutDimensions().minThumbHeight,
                uiScale_);
            addAndMakeVisible(*customScrollBar_);
        }
    }

    void ScrollablePopupMenu::repaintScrollBar()
    {
        if (customScrollBar_ != nullptr)
            customScrollBar_->repaint();
    }

    juce::Rectangle<float> ScrollablePopupMenu::getItemBounds(int itemIndex) const
    {
        if (! isValidItemIndex(itemIndex))
            return juce::Rectangle<float>();

        if (contentComponent_ != nullptr)
        {
            const float itemHeight = static_cast<float>(getItemHeightDesign()) * uiScale_;
            const float y = static_cast<float>(itemIndex) * itemHeight;
            return juce::Rectangle<float>(0.0f, y, columnWidth_, itemHeight);
        }

        return juce::Rectangle<float>();
    }

    int ScrollablePopupMenu::getItemIndexAt(int x, int y) const
    {
        if (contentComponent_ != nullptr)
        {
            if (contentComponent_->getLocalBounds().contains(x, y))
            {
                const float itemHeight = static_cast<float>(getItemHeightDesign()) * uiScale_;
                const int row = static_cast<int>(static_cast<float>(y) / itemHeight);
                if (row >= 0 && row < comboBox_.getNumItems())
                    return row;
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
                renderer_.drawItem(g, comboBox_, i, itemBounds, highlightedItemIndex_, cachedFont_);
        }
    }

    void ScrollablePopupMenu::handleKeyboardNavigation(const juce::KeyPress& key)
    {
        if (key.getKeyCode() == juce::KeyPress::upKey)
            navigateUp();
        else if (key.getKeyCode() == juce::KeyPress::downKey)
            navigateDown();
    }

    void ScrollablePopupMenu::scrollViewportBy(int deltaPixels)
    {
        if (viewport_ == nullptr)
            return;
        const auto* content = viewport_->getViewedComponent();
        if (content == nullptr)
            return;
        const int contentHeight = content->getHeight();
        const int viewportHeight = viewport_->getViewHeight();
        if (contentHeight <= viewportHeight)
            return;
        const int range = contentHeight - viewportHeight;
        const int newY = juce::jlimit(0, range, viewport_->getViewPositionY() + deltaPixels);
        viewport_->setViewPosition(0, newY);
        repaintScrollBar();
    }

    void ScrollablePopupMenu::scrollToHighlightedItem()
    {
        if (viewport_ == nullptr || contentComponent_ == nullptr || highlightedItemIndex_ < 0)
            return;

        const float itemHeight = static_cast<float>(getItemHeightDesign()) * uiScale_;
        const float itemY = static_cast<float>(highlightedItemIndex_) * itemHeight;
        const auto viewportY = viewport_->getViewPositionY();
        const auto viewportHeight = viewport_->getHeight();

        const bool isItemVisible = (itemY >= static_cast<float>(viewportY)
            && itemY + itemHeight <= static_cast<float>(viewportY + viewportHeight));
        if (! isItemVisible)
        {
            const auto centeredY = juce::jmax(0, juce::roundToInt(itemY - static_cast<float>(viewportHeight) * 0.5f));
            viewport_->setViewPosition(0, centeredY);
            repaintScrollBar();
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
            return;

        auto* topLevelComponent = comboBox.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return;

        auto popupMenu = std::make_unique<ScrollablePopupMenu>(comboBox);
        auto* rawPtr = popupMenu.get();

        const auto& popupLayout = ComboBox::getPopupLayoutDimensions();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(comboBox);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            rawPtr->getBorderThicknessDesign(),
            rawPtr->uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kFloor);
        const int insetPx = juce::roundToInt(borderThickness);
        const int scrollbarThicknessPx = juce::jmax(1, juce::roundToInt(
            static_cast<float>(popupLayout.scrollbarWidth) * rawPtr->uiScale_));
        const float maxScrollableHeight = static_cast<float>(popupLayout.maxScrollHeight) * rawPtr->uiScale_;
        const int viewportHeightPx = (rawPtr->scrollableContentHeight_ <= static_cast<int>(maxScrollableHeight))
            ? rawPtr->scrollableContentHeight_
            : juce::roundToInt(maxScrollableHeight);

        const int rightMarginPx = rawPtr->scrollbarNeeded_
            ? juce::jmax(
                juce::roundToInt(kRightMarginFromHighlightToEdge_ * rawPtr->uiScale_),
                scrollbarThicknessPx)
            : 0;
        const int popupWidth = juce::roundToInt(rawPtr->columnWidth_) + rightMarginPx + 2 * insetPx;
        const int popupHeight = viewportHeightPx + 2 * insetPx;

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            comboBox,
            popupWidth,
            popupHeight,
            0);

        topLevelComponent->addAndMakeVisible(popupMenu.release());

        rawPtr->setBounds(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
        rawPtr->resized();
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
    }
}
