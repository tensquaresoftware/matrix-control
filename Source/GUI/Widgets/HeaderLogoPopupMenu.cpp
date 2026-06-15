#include "HeaderLogoPopupMenu.h"

#include "Logo.h"
#include "PopupMenuPositioner.h"
#include "PopupMenuRenderer.h"
#include "ComboBox.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    HeaderLogoPopupMenu::HeaderLogoPopupMenu(ISkin& skin,
                                             float uiScale,
                                             int currentSkinItemId,
                                             int currentUiScaleId,
                                             std::function<void(int skinItemId)> onSkinSelected,
                                             std::function<void(int scaleId)> onUiScaleSelected,
                                             std::function<void()> onSettingsRequested)
        : uiScale_(uiScale)
        , currentSkinItemId_(currentSkinItemId)
        , currentUiScaleId_(currentUiScaleId)
        , onSkinSelected_(std::move(onSkinSelected))
        , onUiScaleSelected_(std::move(onUiScaleSelected))
        , onSettingsRequested_(std::move(onSettingsRequested))
        , look_(popupMenuLookFromSkin(skin))
        , renderer_(std::make_unique<PopupMenuRenderer>(true, uiScale_))
        , cachedFont_(look_.font.withHeight(look_.font.getHeight() * uiScale_))
    {
        renderer_->setLook(look_);
        buildItems();
        setWantsKeyboardFocus(true);
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(true, true);
        setOpaque(true);
    }

    HeaderLogoPopupMenu::~HeaderLogoPopupMenu() = default;

    void HeaderLogoPopupMenu::buildItems()
    {
        items_.clear();

        items_.push_back({ ItemKind::SectionHeader, 0, PluginDisplayNames::HeaderPanel::kLogoUiScaleSection, 0, 0 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k50, PluginDisplayNames::ChoiceLists::ScaleLevels::k50, 0, 1 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k75, PluginDisplayNames::ChoiceLists::ScaleLevels::k75, 0, 2 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k100, PluginDisplayNames::ChoiceLists::ScaleLevels::k100, 0, 3 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k125, PluginDisplayNames::ChoiceLists::ScaleLevels::k125, 0, 4 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k150, PluginDisplayNames::ChoiceLists::ScaleLevels::k150, 0, 5 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k175, PluginDisplayNames::ChoiceLists::ScaleLevels::k175, 0, 6 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k200, PluginDisplayNames::ChoiceLists::ScaleLevels::k200, 0, 7 });

        items_.push_back({ ItemKind::SectionHeader, 0, PluginDisplayNames::HeaderPanel::kLogoSkinSection, 1, 0 });
        items_.push_back({ ItemKind::Skin,
                           static_cast<int>(Skin::SkinComboBoxItemId::kBlack),
                           PluginDisplayNames::ChoiceLists::SkinVariants::kBlack,
                           1, 1 });
        items_.push_back({ ItemKind::Skin,
                           static_cast<int>(Skin::SkinComboBoxItemId::kCream),
                           PluginDisplayNames::ChoiceLists::SkinVariants::kCream,
                           1, 2 });
        items_.push_back({ ItemKind::Spacer, 0, {}, 1, 3 });
        items_.push_back({ ItemKind::HorizontalRule, 0, {}, 1, 4 });
        items_.push_back({ ItemKind::Settings,
                           0,
                           PluginDisplayNames::HeaderPanel::kSettingsButton,
                           1, 5 });
    }

    int HeaderLogoPopupMenu::getItemHeightPx() const
    {
        return juce::jmax(1, juce::roundToInt(
            static_cast<float>(ComboBox::getPopupLayoutDimensions().itemHeight) * uiScale_));
    }

    float HeaderLogoPopupMenu::getSeparatorWidth() const
    {
        return juce::jmax(1.0f, 1.0f * uiScale_);
    }

    float HeaderLogoPopupMenu::getColumnWidth(int /*column*/) const
    {
        return static_cast<float>(kColumnWidthDesign_) * uiScale_;
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
        const float itemHeight = static_cast<float>(getItemHeightPx());

        return {
            contentBounds.getX() + columnOffset,
            contentBounds.getY() + static_cast<float>(item.row) * itemHeight,
            columnWidth,
            itemHeight
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

        const auto kind = items_[static_cast<size_t>(flatIndex)].kind;
        return kind == ItemKind::Skin || kind == ItemKind::UiScale || kind == ItemKind::Settings;
    }

    bool HeaderLogoPopupMenu::isCurrentSelection(int flatIndex) const
    {
        if (! isSelectableItem(flatIndex))
            return false;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (item.kind == ItemKind::Settings)
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

        closePopup();
    }

    void HeaderLogoPopupMenu::closePopup()
    {
        if (auto* modal = getCurrentlyModalComponent())
        {
            if (modal == this)
                exitModalState(0);
        }
    }

    void HeaderLogoPopupMenu::drawItems(juce::Graphics& g, const juce::Rectangle<float>& contentBounds)
    {
        const float separatorWidth = getSeparatorWidth();
        renderer_->drawVerticalSeparators(g,
                                          contentBounds,
                                          kColumnCount_,
                                          getColumnWidth(0),
                                          separatorWidth);

        const float textPadding = static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * uiScale_;
        const float highlightGap = juce::jmax(1.0f, kHighlightGap_ * uiScale_);
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float ruleThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(ComboBox::getPopupLayoutDimensions().borderThickness),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);

        for (int i = 0; i < static_cast<int>(items_.size()); ++i)
        {
            const auto& item = items_[static_cast<size_t>(i)];
            const auto itemBounds = getItemBounds(i);

            if (item.kind == ItemKind::Spacer)
                continue;

            if (item.kind == ItemKind::HorizontalRule)
            {
                g.setColour(look_.borderButtonLike);
                g.fillRect(itemBounds.withSizeKeepingCentre(itemBounds.getWidth(), ruleThickness));
                continue;
            }

            const bool isHighlighted = highlightedFlatIndex_ == i;
            const bool isSelected = isCurrentSelection(i);
            const bool isSectionHeader = item.kind == ItemKind::SectionHeader;

            if ((isHighlighted || isSelected) && ! isSectionHeader)
            {
                g.setColour(look_.backgroundHoverButtonLike);
                g.fillRect(itemBounds.reduced(highlightGap));
            }

            auto textColour = isSectionHeader
                ? look_.textButtonLike.withAlpha(0.5f)
                : look_.textButtonLike;

            if ((isHighlighted || isSelected) && ! isSectionHeader)
                textColour = look_.textHoverButtonLike;

            g.setColour(textColour);
            g.setFont(cachedFont_);
            g.drawText(item.text,
                       itemBounds.withTrimmedLeft(textPadding),
                       juce::Justification::centredLeft,
                       false);
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
        updateHighlightedItem(getFlatIndexAt(e.getPosition().x, e.getPosition().y));
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

    void HeaderLogoPopupMenu::show(Logo& logo,
                                  ISkin& skin,
                                  float uiScale,
                                  int currentSkinItemId,
                                  int currentUiScaleId,
                                  std::function<void(int skinItemId)> onSkinSelected,
                                  std::function<void(int scaleId)> onUiScaleSelected,
                                  std::function<void()> onSettingsRequested)
    {
        auto* topLevelComponent = logo.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return;

        auto popupMenu = std::make_unique<HeaderLogoPopupMenu>(
            skin,
            uiScale,
            currentSkinItemId,
            currentUiScaleId,
            std::move(onSkinSelected),
            std::move(onUiScaleSelected),
            std::move(onSettingsRequested));
        auto* rawPtr = popupMenu.get();

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(logo);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const int insetPx = juce::roundToInt(borderThickness);
        const int itemHeightPx = rawPtr->getItemHeightPx();
        const int maxRows = 8;
        const int separatorPx = juce::roundToInt(rawPtr->getSeparatorWidth());
        const int popupWidth = juce::roundToInt(rawPtr->getColumnWidth(0))
            + separatorPx
            + juce::roundToInt(rawPtr->getColumnWidth(1))
            + 2 * insetPx;
        const int popupHeight = maxRows * itemHeightPx + 2 * insetPx;

        const auto dimensions = PopupMenuPositioner::calculateDimensions(
            logo,
            popupWidth,
            popupHeight,
            ComboBox::getPopupLayoutDimensions().verticalMargin);

        topLevelComponent->addAndMakeVisible(popupMenu.release());
        rawPtr->setBounds(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
        rawPtr->toFront(false);
        rawPtr->grabKeyboardFocus();
        rawPtr->enterModalState(false, nullptr, true);
    }
}
