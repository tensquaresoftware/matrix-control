#include "DialogMatrixHelpers.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"

using TSS::SkinColourId;

namespace DialogMatrixHelpers
{
    std::unique_ptr<TSS::Button> makeButton(TSS::ISkin& skin,
                                            int designWidth,
                                            const juce::String& text,
                                            int designHeight)
    {
        auto button = std::make_unique<TSS::Button>(
            designWidth, designHeight, TSS::buttonLookFromSkin(skin), text);
        button->setMouseClickGrabsKeyboardFocus(false);
        return button;
    }

    void applyButtonSkin(TSS::Button& button, TSS::ISkin& skin)
    {
        button.setLook(TSS::buttonLookFromSkin(skin));
    }

    void applyButtonUiScale(TSS::Button& button, float uiScale)
    {
        button.setUiScale(uiScale);
    }

    juce::Font scaledModalBodyFont(const TSS::ISkin& skin, float uiScale)
    {
        return skin.getModalBodyFont().withHeight(skin.getModalBodyFont().getHeight() * uiScale);
    }

    juce::Font scaledTitleFont(const TSS::ISkin& skin, float uiScale)
    {
        return skin.getBaseFontBold().withHeight(skin.getBaseFontBold().getHeight() * uiScale);
    }

    void paintMatrixOverlayChrome(const OverlayChromePaintArgs& args)
    {
        args.g.fillAll(args.skin.getColour(SkinColourId::kBodyPanelBackground).withAlpha(0.85f));

        args.g.setColour(juce::Colour(kDialogBorderColour));
        args.g.fillRect(args.dialogBounds);

        auto inner = args.dialogBounds.reduced(args.borderThickness);
        auto titleBar = inner.removeFromTop(args.titleBarHeight);
        auto content = inner;

        args.g.setColour(args.skin.getColour(SkinColourId::kHeaderPanelBackground));
        args.g.fillRect(titleBar);
        args.g.fillRect(content);

        args.g.setColour(args.skin.getColour(SkinColourId::kDarkPanelText));
        args.g.setFont(scaledTitleFont(args.skin, args.uiScale));
        args.g.drawText(args.title, titleBar, juce::Justification::centred, false);
    }
}
