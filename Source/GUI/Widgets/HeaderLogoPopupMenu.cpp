#include "HeaderLogoPopupMenu.h"

#include "Logo.h"
#include "PopupMenuPositioner.h"
#include "PopupMenuRenderer.h"
#include "ComboBox.h"

#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    HeaderLogoPopupMenu::HeaderLogoPopupMenu(ISkin& skin, Config config)
        : uiScale_(config.uiScale)
        , currentSkinItemId_(config.currentSkinItemId)
        , currentUiScaleId_(config.currentUiScaleId)
        , onSkinSelected_(std::move(config.onSkinSelected))
        , onUiScaleSelected_(std::move(config.onUiScaleSelected))
        , onAudioMidiSettingsRequested_(std::move(config.onAudioMidiSettingsRequested))
        , onSettingsRequested_(std::move(config.onSettingsRequested))
        , onAboutRequested_(std::move(config.onAboutRequested))
        , showAudioMidiDevices_(onAudioMidiSettingsRequested_ != nullptr)
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
        appendUiScaleColumnItems();
        appendSkinAndActionColumnItems();
    }

    void HeaderLogoPopupMenu::appendUiScaleColumnItems()
    {
        items_.push_back({ ItemKind::SectionHeader,
                           0,
                           PluginDisplayNames::HeaderPanel::kLogoUiScaleSection,
                           TSS::EditorChromeShortcutLabels::uiScaleStep(),
                           0,
                           0 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k50,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k50, {}, 0, 1 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k75,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k75, {}, 0, 2 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k100,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k100,
                           TSS::EditorChromeShortcutLabels::uiScaleReset(), 0, 3 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k125,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k125, {}, 0, 4 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k150,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k150, {}, 0, 5 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k175,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k175, {}, 0, 6 });
        items_.push_back({ ItemKind::UiScale, PluginIDs::Settings::ScaleLevels::k200,
                           PluginDisplayNames::ChoiceLists::ScaleLevels::k200, {}, 0, 7 });
    }

    void HeaderLogoPopupMenu::appendSkinAndActionColumnItems()
    {
        items_.push_back({ ItemKind::SectionHeader, 0, PluginDisplayNames::HeaderPanel::kLogoSkinSection, {}, 1, 0 });
        items_.push_back({ ItemKind::Skin,
                           static_cast<int>(Skin::SkinComboBoxItemId::kBlack),
                           PluginDisplayNames::ChoiceLists::SkinVariants::kBlack,
                           {},
                           1, 1 });
        items_.push_back({ ItemKind::Skin,
                           static_cast<int>(Skin::SkinComboBoxItemId::kCream),
                           PluginDisplayNames::ChoiceLists::SkinVariants::kCream,
                           {},
                           1, 2 });
        items_.push_back({ ItemKind::Spacer, 0, {}, {}, 1, 3 });
        items_.push_back({ ItemKind::HorizontalRule, 0, {}, {}, 1, 4 });

        int actionRow = 5;
        if (showAudioMidiDevices_)
        {
            items_.push_back({ ItemKind::AudioMidiDevices,
                               0,
                               PluginDisplayNames::HeaderPanel::kAudioMidiButton,
                               TSS::EditorChromeShortcutLabels::openAudioMidi(),
                               1, actionRow++ });
        }

        items_.push_back({ ItemKind::Settings,
                           0,
                           PluginDisplayNames::HeaderPanel::kSettingsButton,
                           TSS::EditorChromeShortcutLabels::openSettings(),
                           1, actionRow++ });
        items_.push_back({ ItemKind::About,
                           0,
                           PluginDisplayNames::HeaderPanel::kAboutButton,
                           {},
                           1, actionRow++ });
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

    float HeaderLogoPopupMenu::getColumnWidth(int column) const
    {
        const int designWidth = column == 0 ? kColumnWidthDesign_ : kActionColumnWidthDesign_;
        return static_cast<float>(designWidth) * uiScale_;
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
        return kind == ItemKind::Skin || kind == ItemKind::UiScale || kind == ItemKind::AudioMidiDevices
            || kind == ItemKind::Settings || kind == ItemKind::About;
    }

    bool HeaderLogoPopupMenu::isCurrentSelection(int flatIndex) const
    {
        if (! isSelectableItem(flatIndex))
            return false;

        const auto& item = items_[static_cast<size_t>(flatIndex)];
        if (item.kind == ItemKind::Settings || item.kind == ItemKind::About || item.kind == ItemKind::AudioMidiDevices)
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
        else if (item.kind == ItemKind::AudioMidiDevices)
        {
            if (onAudioMidiSettingsRequested_)
                onAudioMidiSettingsRequested_();
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
            g.setColour(look_.borderButtonLike);
            g.fillRect(itemBounds.withSizeKeepingCentre(itemBounds.getWidth(), metrics.ruleThickness));
            return;
        }

        const bool isHighlighted = highlightedFlatIndex_ == flatIndex;
        const bool isSelected = isCurrentSelection(flatIndex);
        const bool isSectionHeader = item.kind == ItemKind::SectionHeader;

        if ((isHighlighted || isSelected) && ! isSectionHeader)
        {
            g.setColour(look_.backgroundHoverButtonLike);
            g.fillRect(itemBounds.reduced(metrics.highlightGap));
        }

        auto textColour = isSectionHeader ? look_.textButtonLike.withAlpha(0.5f) : look_.textButtonLike;
        if ((isHighlighted || isSelected) && ! isSectionHeader)
            textColour = look_.textHoverButtonLike;

        g.setColour(textColour);
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

    juce::Rectangle<int> HeaderLogoPopupMenu::preferredBoundsOver(Logo& logo) const
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(logo);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const int insetPx = juce::roundToInt(borderThickness);
        const int maxRows = showAudioMidiDevices_ ? 9 : 8;
        const int separatorPx = juce::roundToInt(getSeparatorWidth());
        const int popupWidth = juce::roundToInt(getColumnWidth(0))
            + separatorPx
            + juce::roundToInt(getColumnWidth(1))
            + 2 * insetPx;
        const int popupHeight = maxRows * getItemHeightPx() + 2 * insetPx;

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
    }
}
