#include "HeaderLogoPopupMenu.h"

#include "ComboBox.h"
#include "Logo.h"
#include "PopupMenuPositioner.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ColourChart.h"

namespace TSS
{
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
        const auto contentBounds = juce::Rectangle<float>(0.0f, 0.0f,
                                                          static_cast<float>(mainPopupWidth_),
                                                          static_cast<float>(mainPopupHeight_))
                                       .reduced(borderThickness);

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
        if (x >= mainPopupWidth_)
            return -1;

        for (int i = 0; i < static_cast<int>(items_.size()); ++i)
        {
            if (getItemBounds(i).contains(static_cast<float>(x), static_cast<float>(y)))
                return i;
        }

        return -1;
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

}
