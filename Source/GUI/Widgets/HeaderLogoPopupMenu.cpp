#include "HeaderLogoPopupMenu.h"

#include "Logo.h"
#include "PopupMenuOutsideDismissWatcher.h"
#include "PopupMenuPositioner.h"
#include "PopupMenuRenderer.h"
#include "ComboBox.h"

#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    namespace
    {
        juce::Colour menuItemTextColour(const PopupMenuLook& look,
                                        bool muted,
                                        bool hot) noexcept
        {
            if (muted)
                return look.textDisabled;
            if (hot)
                return look.textHover;
            return look.text;
        }
    }

    HeaderLogoPopupMenu::HeaderLogoPopupMenu(ISkin& skin, Config config)
        : uiScale_(config.uiScale)
        , currentSkinItemId_(config.currentSkinItemId)
        , currentUiScaleId_(config.currentUiScaleId)
        , onSkinSelected_(std::move(config.onSkinSelected))
        , onUiScaleSelected_(std::move(config.onUiScaleSelected))
        , onSettingsRequested_(std::move(config.onSettingsRequested))
        , onAboutRequested_(std::move(config.onAboutRequested))
        , contextualHelpBinder_(config.contextualHelpBinder)
        , look_(popupMenuLookFromSkin(skin,
                                       zoneColourSet(SkinObjectType::kPopupMenu, kMonoMenuExceptionZone)))
        , renderer_(std::make_unique<PopupMenuRenderer>(uiScale_))
        , cachedFont_(look_.font.withHeight(look_.font.getHeight() * uiScale_))
    {
        renderer_->setLook(look_);
        buildItems();
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        setOpaque(true);

        outsideDismissWatcher_ = std::make_unique<PopupMenuOutsideDismissWatcher>(*this, [this] { closePopup(); });
    }

    HeaderLogoPopupMenu::~HeaderLogoPopupMenu() = default;

    int HeaderLogoPopupMenu::getItemHeightPx() const
    {
        return juce::jmax(1, juce::roundToInt(
            static_cast<float>(ComboBox::getPopupLayoutDimensions().itemHeight) * uiScale_));
    }

    float HeaderLogoPopupMenu::getRowHeightPx(const MenuItem& item) const
    {
        if (item.kind == ItemKind::HorizontalRule)
        {
            const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
            return ScaledDrawing::snappedStrokeThicknessFromDesign(
                static_cast<float>(ComboBox::getPopupLayoutDimensions().borderThickness),
                uiScale_,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kRound);
        }

        return static_cast<float>(getItemHeightPx());
    }

    float HeaderLogoPopupMenu::columnContentHeightForDisplayScale(int column, float systemDisplayScale) const
    {
        const float ruleHeight = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(ComboBox::getPopupLayoutDimensions().borderThickness),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const float itemHeight = static_cast<float>(getItemHeightPx());

        float height = 0.0f;
        for (const auto& item : items_)
        {
            if (item.column != column)
                continue;
            height += item.kind == ItemKind::HorizontalRule ? ruleHeight : itemHeight;
        }
        return height;
    }

    float HeaderLogoPopupMenu::getSeparatorWidth() const
    {
        return juce::jmax(1.0f, 1.0f * uiScale_);
    }

    float HeaderLogoPopupMenu::getColumnWidth(int column) const
    {
        const int designWidth = column == 0 ? kColumnWidthDesign_ : kActionColumnWidthDesign_;
        return static_cast<float>(designWidth) * uiScale_;
    }

    juce::Colour HeaderLogoPopupMenu::sectionHeaderTextColour() const
    {
        // Cream logo plate: section labels need more contrast than disabled rows (Cafe300).
        if (look_.background == juce::Colour(ColourChart::kBrandCream))
            return juce::Colour(ColourChart::kCafe500);

        return look_.textDisabled;
    }

    juce::Rectangle<float> HeaderLogoPopupMenu::getItemBounds(int flatIndex) const
    {
        if (! juce::isPositiveAndBelow(flatIndex, static_cast<int>(items_.size())))
            return {};

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const auto contentBounds = getLocalBounds().toFloat().reduced(borderThickness);

        const float separatorWidth = getSeparatorWidth();
        const float columnWidth = getColumnWidth(item.column);
        const float columnOffset = item.column == 0
            ? 0.0f
            : getColumnWidth(0) + separatorWidth;

        float y = contentBounds.getY();
        for (int i = 0; i < flatIndex; ++i)
        {
            const auto& prior = items_[static_cast<size_t>(i)];
            if (prior.column == item.column)
                y += getRowHeightPx(prior);
        }

        return {
            contentBounds.getX() + columnOffset,
            y,
            columnWidth,
            getRowHeightPx(item)
        };
    }

    int HeaderLogoPopupMenu::getFlatIndexAt(int x, int y) const
    {
        for (int i = 0; i < static_cast<int>(items_.size()); ++i)
        {
            if (getItemBounds(i).contains(static_cast<float>(x), static_cast<float>(y)))
                return i;
        }

        return -1;
    }

    bool HeaderLogoPopupMenu::isSelectableItem(int flatIndex) const
    {
        if (! juce::isPositiveAndBelow(flatIndex, static_cast<int>(items_.size())))
            return false;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (! item.enabled)
            return false;

        return item.kind == ItemKind::Skin || item.kind == ItemKind::UiScale
            || item.kind == ItemKind::Settings || item.kind == ItemKind::About;
    }

    bool HeaderLogoPopupMenu::isCurrentSelection(int flatIndex) const
    {
        if (! isSelectableItem(flatIndex))
            return false;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (item.kind == ItemKind::Settings || item.kind == ItemKind::About)
            return false;

        if (item.kind == ItemKind::Skin)
            return item.valueId == currentSkinItemId_;

        return item.valueId == currentUiScaleId_;
    }

    void HeaderLogoPopupMenu::updateHighlightedItem(int flatIndex)
    {
        if (flatIndex >= 0 && ! isSelectableItem(flatIndex))
            flatIndex = -1;

        if (highlightedFlatIndex_ == flatIndex)
            return;

        highlightedFlatIndex_ = flatIndex;
        repaint();
    }

    const char* HeaderLogoPopupMenu::contextualHelpTextForItem(int flatIndex) const
    {
        if (! juce::isPositiveAndBelow(flatIndex, static_cast<int>(items_.size())))
            return nullptr;

        namespace Help = PluginDisplayNames::HeaderPanel::ContextualHelp;
        switch (items_[static_cast<size_t>(flatIndex)].kind)
        {
            case ItemKind::Settings:         return Help::kSettings;
            case ItemKind::About:            return Help::kAbout;
            case ItemKind::Skin:             return Help::kSkin;
            case ItemKind::UiScale:          return Help::kUiScale;
            case ItemKind::SectionHeader:
                if (items_[static_cast<size_t>(flatIndex)].text
                    == PluginDisplayNames::HeaderPanel::kLogoSkinSection)
                    return Help::kSkin;
                if (items_[static_cast<size_t>(flatIndex)].text
                    == PluginDisplayNames::HeaderPanel::kLogoUiScaleSection)
                    return Help::kUiScale;
                return nullptr;
            case ItemKind::Spacer:
            case ItemKind::HorizontalRule:
                return nullptr;
        }

        return nullptr;
    }

    void HeaderLogoPopupMenu::updateContextualHelpForItem(int flatIndex)
    {
        if (contextualHelpBinder_ == nullptr)
            return;

        if (const auto* helpText = contextualHelpTextForItem(flatIndex))
            contextualHelpBinder_->showHelpText(helpText);
        else
            contextualHelpBinder_->scheduleClear();
    }

    void HeaderLogoPopupMenu::selectItem(int flatIndex)
    {
        if (! isSelectableItem(flatIndex))
            return;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (item.kind == ItemKind::Skin)
        {
            if (onSkinSelected_)
                onSkinSelected_(item.valueId);
        }
        else if (item.kind == ItemKind::UiScale)
        {
            if (onUiScaleSelected_)
                onUiScaleSelected_(item.valueId);
        }
        else if (item.kind == ItemKind::Settings)
        {
            if (onSettingsRequested_)
                onSettingsRequested_();
        }
        else if (item.kind == ItemKind::About)
        {
            if (onAboutRequested_)
                onAboutRequested_();
        }

        closePopup();
    }

    void HeaderLogoPopupMenu::closePopup()
    {
        if (contextualHelpBinder_ != nullptr)
            contextualHelpBinder_->scheduleClear();

        if (auto* modal = getCurrentlyModalComponent())
        {
            if (modal == this)
                exitModalState(0);
        }
    }

    void HeaderLogoPopupMenu::detachContextualHelpBinder()
    {
        contextualHelpBinder_ = nullptr;
    }

    void HeaderLogoPopupMenu::drawItems(juce::Graphics& g, const juce::Rectangle<float>& contentBounds)
    {
        const float separatorWidth = getSeparatorWidth();
        renderer_->drawVerticalSeparators(g, {
            .contentBounds = contentBounds,
            .columnCount = kColumnCount_,
            .actualColumnWidth = getColumnWidth(0),
            .separatorWidth = separatorWidth});

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const ItemDrawMetrics metrics{
            .textPadding = static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * uiScale_,
            .highlightGap = juce::jmax(1.0f, kHighlightGap_ * uiScale_),
            .ruleThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
                static_cast<float>(ComboBox::getPopupLayoutDimensions().borderThickness),
                uiScale_,
                systemDisplayScale,
                ScaledDrawing::StrokeSnapPolicy::kRound)};

        for (int i = 0; i < static_cast<int>(items_.size()); ++i)
            drawMenuItem(g, i, metrics);
    }

    void HeaderLogoPopupMenu::drawMenuItem(juce::Graphics& g, int flatIndex, const ItemDrawMetrics& metrics)
    {
        const auto& item = items_[static_cast<size_t>(flatIndex)];
        const auto itemBounds = getItemBounds(flatIndex);

        if (item.kind == ItemKind::Spacer)
            return;

        if (item.kind == ItemKind::HorizontalRule)
        {
            g.setColour(look_.border);
            g.fillRect(itemBounds.withHeight(metrics.ruleThickness));
            return;
        }

        const bool isSectionHeader = item.kind == ItemKind::SectionHeader;
        const bool isDisabledRow = ! item.enabled;
        const bool isHot = ! isSectionHeader && ! isDisabledRow
            && (highlightedFlatIndex_ == flatIndex || isCurrentSelection(flatIndex));

        if (isHot)
        {
            g.setColour(look_.backgroundHover);
            g.fillRect(itemBounds.reduced(metrics.highlightGap));
        }

        if (isSectionHeader)
            g.setColour(sectionHeaderTextColour());
        else
            g.setColour(menuItemTextColour(look_, isDisabledRow, isHot));
        g.setFont(cachedFont_);

        const auto textBounds = itemBounds.withTrimmedLeft(metrics.textPadding)
                                    .withTrimmedRight(metrics.textPadding);
        g.drawText(item.text, textBounds, juce::Justification::centredLeft, false);

        if (item.shortcut.isNotEmpty())
        {
            g.setFont(TSS::shortcutHintFont(cachedFont_.getHeight()));
            g.drawText(item.shortcut, textBounds, juce::Justification::centredRight, false);
        }
    }

    void HeaderLogoPopupMenu::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        renderer_->drawBackground(g, bounds);

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const auto contentBounds = bounds.reduced(borderThickness);

        drawItems(g, contentBounds);
        renderer_->drawBorder(g, bounds, systemDisplayScale);
    }

    void HeaderLogoPopupMenu::mouseMove(const juce::MouseEvent& e)
    {
        const int flatIndex = getFlatIndexAt(e.getPosition().x, e.getPosition().y);
        updateHighlightedItem(flatIndex);
        updateContextualHelpForItem(flatIndex);
    }

    void HeaderLogoPopupMenu::mouseUp(const juce::MouseEvent& e)
    {
        const auto flatIndex = getFlatIndexAt(e.getPosition().x, e.getPosition().y);
        if (flatIndex >= 0)
            selectItem(flatIndex);
    }

    void HeaderLogoPopupMenu::mouseExit(const juce::MouseEvent&)
    {
        updateHighlightedItem(-1);
        updateContextualHelpForItem(-1);
    }

    bool HeaderLogoPopupMenu::keyPressed(const juce::KeyPress& key)
    {
        if (key.getKeyCode() == juce::KeyPress::escapeKey)
        {
            closePopup();
            return true;
        }

        return false;
    }

    void HeaderLogoPopupMenu::inputAttemptWhenModal()
    {
        closePopup();
    }

    juce::Rectangle<int> HeaderLogoPopupMenu::preferredBoundsOver(Logo& logo) const
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(logo);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const int insetPx = juce::roundToInt(borderThickness);
        const int separatorPx = juce::roundToInt(getSeparatorWidth());
        const int popupWidth = juce::roundToInt(getColumnWidth(0))
            + separatorPx
            + juce::roundToInt(getColumnWidth(1))
            + 2 * insetPx;
        const float contentHeight = juce::jmax(
            columnContentHeightForDisplayScale(0, systemDisplayScale),
            columnContentHeightForDisplayScale(1, systemDisplayScale));
        const int popupHeight = juce::jmax(1, juce::roundToInt(contentHeight)) + 2 * insetPx;

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            logo,
            PopupMenuPositioner::DimensionsArgs{
                .popupWidth = popupWidth,
                .popupHeight = popupHeight,
                .verticalMargin = ComboBox::getPopupLayoutDimensions().verticalMargin});

        return { dimensions.x, dimensions.y, dimensions.width, dimensions.height };
    }

    void HeaderLogoPopupMenu::show(Logo& logo, ISkin& skin, Config config)
    {
        auto* topLevelComponent = logo.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return;

        auto popupMenu = std::make_unique<HeaderLogoPopupMenu>(skin, std::move(config));
        auto* rawPtr = popupMenu.get();
        const auto bounds = rawPtr->preferredBoundsOver(logo);

        topLevelComponent->addAndMakeVisible(popupMenu.release());
        rawPtr->setBounds(bounds);
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
        rawPtr->outsideDismissWatcher_->arm();
    }
}
