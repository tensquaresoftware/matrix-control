#include "ScrollablePopupMenu.h"
#include "ComboBox.h"
#include "PopupMenuPositioner.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ColourChart.h"

namespace TSS
{
    namespace
    {
        /** Integer logical px for popup chrome (border / sentinel rule), matching the
            physical-pixel snap then rounding so layout and paint share one grid. */
        int popupChromeStrokePx(float uiScale, float systemDisplayScale, float designThickness)
        {
            const float snapped = ScaledDrawing::snappedStrokeThicknessFromDesign(
                designThickness,
                uiScale,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kFloor);
            return juce::jmax(1, juce::roundToInt(snapped));
        }
    }

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
        columnWidth_ = static_cast<float>(juce::jmax(1, comboBox_.getWidth()));
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
        const int strokePx = popupChromeStrokePx(uiScale_, systemDisplayScale, getBorderThicknessDesign());
        const float stroke = static_cast<float>(strokePx);

        // Full fill under an integer stroke avoids fractional gaps (dark "liseret") between
        // background and border at 150%/175%.
        renderer_.drawBackground(g, bounds);
        g.setColour(isButtonLike_ ? comboBox_.getPopupMenuLook().borderButtonLike
                                  : comboBox_.getPopupMenuLook().border);
        g.drawRect(bounds, stroke);
    }

    void ScrollablePopupMenu::paintOverChildren(juce::Graphics& g)
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const int strokePx = popupChromeStrokePx(uiScale_, systemDisplayScale, getBorderThicknessDesign());
        drawSentinelRuleIfNeeded(g,
                                 getLocalBounds().toFloat().reduced(static_cast<float>(strokePx)),
                                 static_cast<float>(strokePx));
    }

    void ScrollablePopupMenu::drawSentinelRuleIfNeeded(juce::Graphics& g,
                                                       const juce::Rectangle<float>& contentBounds,
                                                       float ruleThickness)
    {
        constexpr int kPortSentinelItemId = 1;
        if (comboBox_.getNumItems() <= 1 || comboBox_.getItemId(0) != kPortSentinelItemId)
            return;
        if (viewport_ == nullptr)
            return;

        const float itemHeight = static_cast<float>(getItemHeightDesign()) * uiScale_;
        const float scrollY = static_cast<float>(viewport_->getViewPositionY());
        // Viewport origin matches the integer content box (same stroke as chrome).
        const float ruleBottom = static_cast<float>(viewport_->getY()) + itemHeight - scrollY;
        if (ruleBottom < contentBounds.getY() + ruleThickness
            || ruleBottom > contentBounds.getBottom())
        {
            return;
        }

        g.setColour(comboBox_.getPopupMenuLook().borderButtonLike);
        g.fillRect(contentBounds.getX(),
                   ruleBottom - ruleThickness,
                   contentBounds.getWidth(),
                   ruleThickness);
    }

    void ScrollablePopupMenu::resized()
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const int strokePx = popupChromeStrokePx(uiScale_, systemDisplayScale, getBorderThicknessDesign());
        const auto inner = getLocalBounds().reduced(strokePx);
        if (viewport_ == nullptr)
            return;

        viewport_->setBounds(inner);

        if (scrollbarNeeded_ && customScrollBar_ != nullptr)
        {
            const int scrollbarThicknessPx = juce::jmax(1, juce::roundToInt(
                static_cast<float>(ComboBox::getPopupLayoutDimensions().scrollbarWidth) * uiScale_));
            customScrollBar_->setBounds(inner.withTrimmedLeft(inner.getWidth() - scrollbarThicknessPx));
            customScrollBar_->toFront(false);
        }

        columnWidth_ = static_cast<float>(viewport_->getWidth());
        if (contentComponent_ != nullptr)
            contentComponent_->setSize(viewport_->getWidth(), scrollableContentHeight_);
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
            const float width = static_cast<float>(juce::jmax(1, contentComponent_->getWidth()));
            return juce::Rectangle<float>(0.0f, y, width, itemHeight);
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
        constexpr int kPortSentinelItemId = 1;
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float ruleThickness = static_cast<float>(
            popupChromeStrokePx(uiScale_, systemDisplayScale, getBorderThicknessDesign()));

        for (int i = 0; i < numItems; ++i)
        {
            const auto itemBounds = getItemBounds(i);
            if (itemBounds.isEmpty())
                continue;

            const bool sentinelWithRule = (i == 0 && numItems > 1
                                           && comboBox_.getItemId(0) == kPortSentinelItemId);

            // reduced(gap) + trim ruleThickness: equal air above hover and above the rule.
            // Gap comes from PopupMenuRenderer (integer HeaderLogo-style air).
            renderer_.drawItem(g, {
                .comboBox = comboBox_,
                .itemIndex = i,
                .itemBounds = itemBounds,
                .highlightedItemIndex = highlightedItemIndex_,
                .font = cachedFont_,
                .systemDisplayScale = systemDisplayScale,
                .highlightBottomExtraTrim = sentinelWithRule ? ruleThickness : 0.0f});
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
        repaint();
    }

    void ScrollablePopupMenu::scrollToHighlightedItem()
    {
        if (viewport_ == nullptr || contentComponent_ == nullptr || highlightedItemIndex_ < 0)
            return;

        const float itemHeight = static_cast<float>(getItemHeightDesign()) * uiScale_;
        const float itemY = static_cast<float>(highlightedItemIndex_) * itemHeight;
        const int viewportY = viewport_->getViewPositionY();
        const int viewportHeight = viewport_->getHeight();
        const bool isItemVisible =
            (itemY >= static_cast<float>(viewportY)
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
        if (! comboBox.canShowPopup())
            return;

        auto* topLevelComponent = comboBox.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return;

        comboBox.notifyPopupOpened();

        auto popupMenu = std::make_unique<ScrollablePopupMenu>(comboBox);
        auto* rawPtr = popupMenu.get();
        comboBox.attachOpenPopup(*rawPtr);

        const auto& popupLayout = ComboBox::getPopupLayoutDimensions();
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(comboBox);
        const int strokePx = popupChromeStrokePx(
            rawPtr->uiScale_,
            systemDisplayScale,
            rawPtr->getBorderThicknessDesign());
        const float maxScrollableHeight = static_cast<float>(popupLayout.maxScrollHeight) * rawPtr->uiScale_;
        const int viewportHeightPx = (rawPtr->scrollableContentHeight_ <= static_cast<int>(maxScrollableHeight))
            ? rawPtr->scrollableContentHeight_
            : juce::roundToInt(maxScrollableHeight);

        const int popupWidth = juce::roundToInt(rawPtr->columnWidth_) + 2 * strokePx;
        const int popupHeight = viewportHeightPx + 2 * strokePx;

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
