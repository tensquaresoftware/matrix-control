#include "HeaderLogoPopupMenu.h"

#include "ComboBox.h"
#include "PopupMenuRenderer.h"

#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Layout/ScaledDrawing.h"

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

    HeaderLogoPopupMenu::FamilyFlyout::FamilyFlyout(HeaderLogoPopupMenu& owner,
                                                    Skin::SkinFamily family,
                                                    std::vector<FamilyMember> members)
        : owner_(owner)
        , family_(family)
        , members_(std::move(members))
    {
        setOpaque(true);
        setInterceptsMouseClicks(true, true);
    }

    int HeaderLogoPopupMenu::FamilyFlyout::preferredWidth() const
    {
        const float border = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            owner_.uiScale_,
            ScaledDrawing::systemDisplayScaleForComponent(owner_),
            ScaledDrawing::StrokeSnapPolicy::kRound);
        return juce::roundToInt(owner_.getColumnWidth(1) + 2.0f * border);
    }

    int HeaderLogoPopupMenu::FamilyFlyout::preferredHeight() const
    {
        const float border = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            owner_.uiScale_,
            ScaledDrawing::systemDisplayScaleForComponent(owner_),
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const float content = static_cast<float>(members_.size())
            * static_cast<float>(owner_.getItemHeightPx());
        return juce::jmax(1, juce::roundToInt(content + 2.0f * border));
    }

    juce::Rectangle<float> HeaderLogoPopupMenu::FamilyFlyout::memberBounds(int index) const
    {
        if (! juce::isPositiveAndBelow(index, static_cast<int>(members_.size())))
            return {};

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            kBorderThicknessDesign_,
            owner_.uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const auto content = getLocalBounds().toFloat().reduced(borderThickness);
        const float itemHeight = static_cast<float>(owner_.getItemHeightPx());
        return { content.getX(), content.getY() + static_cast<float>(index) * itemHeight,
                 content.getWidth(), itemHeight };
    }

    int HeaderLogoPopupMenu::FamilyFlyout::memberIndexAt(int x, int y) const
    {
        for (int i = 0; i < static_cast<int>(members_.size()); ++i)
            if (memberBounds(i).contains(static_cast<float>(x), static_cast<float>(y)))
                return i;
        return -1;
    }

    void HeaderLogoPopupMenu::FamilyFlyout::updateHighlight(int index)
    {
        if (highlightedIndex_ == index)
            return;
        highlightedIndex_ = index;
        repaint();
    }

    void HeaderLogoPopupMenu::FamilyFlyout::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        owner_.renderer_->drawBackground(g, bounds);

        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float textPadding =
            static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * owner_.uiScale_;
        const float highlightGap = juce::jmax(1.0f, kHighlightGap_ * owner_.uiScale_);

        for (int i = 0; i < static_cast<int>(members_.size()); ++i)
        {
            const auto& member = members_[static_cast<size_t>(i)];
            const auto itemBounds = memberBounds(i);
            const bool isCurrent = member.skinItemId == owner_.currentSkinItemId_;
            const bool isHot = highlightedIndex_ == i || isCurrent;

            if (isHot)
            {
                g.setColour(owner_.look_.backgroundHover);
                g.fillRect(itemBounds.reduced(highlightGap));
            }

            g.setColour(menuItemTextColour(owner_.look_, false, isHot));
            g.setFont(owner_.cachedFont_);
            const auto textBounds = itemBounds.withTrimmedLeft(textPadding).withTrimmedRight(textPadding);
            g.drawText(member.text, textBounds, juce::Justification::centredLeft, false);

            if (member.shortcut.isNotEmpty())
            {
                g.setFont(TSS::shortcutHintFont(owner_.cachedFont_.getHeight()));
                g.drawText(member.shortcut, textBounds, juce::Justification::centredRight, false);
            }
        }

        owner_.renderer_->drawBorder(g, bounds, systemDisplayScale);
    }

    void HeaderLogoPopupMenu::FamilyFlyout::mouseMove(const juce::MouseEvent& e)
    {
        updateHighlight(memberIndexAt(e.x, e.y));
    }

    void HeaderLogoPopupMenu::FamilyFlyout::mouseUp(const juce::MouseEvent& e)
    {
        const int index = memberIndexAt(e.x, e.y);
        if (! juce::isPositiveAndBelow(index, static_cast<int>(members_.size())))
            return;
        owner_.selectSkinItemId(members_[static_cast<size_t>(index)].skinItemId);
    }

    void HeaderLogoPopupMenu::FamilyFlyout::mouseExit(const juce::MouseEvent&)
    {
        updateHighlight(-1);
    }

}
