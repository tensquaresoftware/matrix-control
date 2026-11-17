#include "HeaderLogoPopupMenu.h"

#include "ComboBox.h"
#include "PopupMenuRenderer.h"

#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ColourChart.h"

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

        if (item.kind == ItemKind::SkinFamily)
        {
            renderer_->drawSubMenuChevron(g, itemBounds, cachedFont_);
        }
        else if (item.shortcut.isNotEmpty())
        {
            g.setFont(TSS::shortcutHintFont(cachedFont_.getHeight()));
            g.drawText(item.shortcut, textBounds, juce::Justification::centredRight, false);
        }
    }

    void HeaderLogoPopupMenu::paint(juce::Graphics& g)
    {
        const auto mainBounds = juce::Rectangle<float>(0.0f, 0.0f,
                                                       static_cast<float>(mainPopupWidth_),
                                                       static_cast<float>(mainPopupHeight_));
        renderer_->drawBackground(g, mainBounds);

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const auto contentBounds = mainBounds.reduced(borderThickness);

        drawItems(g, contentBounds);
        renderer_->drawBorder(g, mainBounds, systemDisplayScale);
    }

}
