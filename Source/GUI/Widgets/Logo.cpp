#include "Logo.h"

#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    Logo::Logo(ISkin& skin)
        : Label(Design::Panels::Header::kLogoWidth,
                Design::Panels::Header::kLogoHeight,
                brandLabelLookFromSkin(skin),
                PluginDisplayNames::kPluginName)
        , skin_(&skin)
    {
        setInterceptsMouseClicks(true, false);
    }

    void Logo::setSkin(ISkin& skin)
    {
        skin_ = &skin;
        setLook(brandLabelLookFromSkin(skin));
        applyTextColour();
    }

    void Logo::setHighlighted(bool highlighted)
    {
        if (isHighlighted_ == highlighted)
            return;

        isHighlighted_ = highlighted;
        applyTextColour();
    }

    void Logo::applyTextColour()
    {
        if (skin_ == nullptr)
            return;

        auto look = brandLabelLookFromSkin(*skin_);
        look.text = isHighlighted_
                        ? juce::Colour(ColourChart::kWhite)
                        : skin_->getColour(SkinColourId::kLabelText);
        setLook(look);
    }

    void Logo::mouseUp(const juce::MouseEvent& e)
    {
        if (e.getNumberOfClicks() > 1)
            return;

        if (e.mods.isShiftDown() && e.mods.isCtrlDown())
        {
            if (onUiTestsToggleRequested)
                onUiTestsToggleRequested();
            return;
        }

        if (e.mods.isShiftDown())
        {
            if (onSettingsRequested)
                onSettingsRequested();
            return;
        }

        startTimer(200);
    }

    void Logo::mouseDoubleClick(const juce::MouseEvent&)
    {
        stopTimer();

        if (onUiScaleReset)
            onUiScaleReset();
    }

    void Logo::timerCallback()
    {
        stopTimer();

        if (onPopupRequested)
            onPopupRequested();
    }

    void Logo::mouseEnter(const juce::MouseEvent&)
    {
        setHighlighted(true);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }

    void Logo::mouseExit(const juce::MouseEvent&)
    {
        setHighlighted(false);
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
}
