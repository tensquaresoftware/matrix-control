#include "HeaderPanel.h"

#include <memory>

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Widgets/HeaderLogoPopupMenu.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Helpers/InputGainSliderText.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

namespace
{
    void dismissOpenHeaderLogoPopupMenus()
    {
        for (int i = juce::Component::getNumCurrentlyModalComponents(); --i >= 0;)
        {
            if (auto* menu = dynamic_cast<TSS::HeaderLogoPopupMenu*>(
                    juce::Component::getCurrentlyModalComponent(i)))
            {
                menu->detachContextualHelpBinder();
                menu->exitModalState(0);
            }
        }
    }

    struct CartouchePaintArgs
    {
        TSS::ISkin* skin = nullptr;
        float uiScale = 1.0f;
        juce::Rectangle<int> badgeBounds;
        juce::Rectangle<int> frameBounds;
        int strokePx = 1;
        juce::String badgeText;
    };

    void paintCartoucheChrome(juce::Graphics& g, const CartouchePaintArgs& args)
    {
        if (args.skin == nullptr || args.frameBounds.isEmpty())
            return;

        // Badge/frame: LightGrey1; title text: header background (reads as cut-out on the badge).
        const auto cartoucheChrome = juce::Colour(ColourChart::kLightGrey1);
        const auto badgeTextColour = args.skin->getColour(SkinColourId::kHeaderPanelBackground);
        const int stroke = juce::jmax(1, args.strokePx);

        g.setColour(cartoucheChrome);
        g.fillRect(args.badgeBounds);

        auto look = TSS::darkPanelLabelLookFromSkin(*args.skin);
        look.text = badgeTextColour;
        g.setColour(look.text);
        g.setFont(look.font.withHeight(look.font.getHeight() * args.uiScale).boldened());
        g.drawText(args.badgeText, args.badgeBounds, juce::Justification::centred, false);

        g.setColour(cartoucheChrome);
        g.fillRect(args.frameBounds.getX(), args.frameBounds.getY(), args.frameBounds.getWidth(), stroke);
        g.fillRect(args.frameBounds.getX(),
                   args.frameBounds.getBottom() - stroke,
                   args.frameBounds.getWidth(),
                   stroke);
        g.fillRect(args.frameBounds.getRight() - stroke,
                   args.frameBounds.getY(),
                   stroke,
                   args.frameBounds.getHeight());
    }
}

HeaderPanel::~HeaderPanel()
{
    dismissOpenHeaderLogoPopupMenus();
    contextualHelpBinder_.reset();
}

HeaderPanel::HeaderPanel(TSS::ISkin& skin, const HeaderPanelDimensions& dimensions)
    : dimensions_(dimensions)
    , skin_(&skin)
    , logo_(skin, dimensions.logoWidth, dimensions.logoHeight)
    , instrumentActivityLed_(dimensions.ledSize, dimensions.ledSize)
    , keyboardFromLabel_(dimensions.fromKeyboardLabelWidth,
                         dimensions.controlHeight,
                         TSS::darkPanelLabelLookFromSkin(skin),
                         PluginDisplayNames::HeaderPanel::kFromKeyboardLabel)
    , editorActivityLed_(dimensions.ledSize, dimensions.ledSize)
    , midiFromLabel_(dimensions.fromSynthLabelWidth,
                     dimensions.controlHeight,
                     TSS::darkPanelLabelLookFromSkin(skin),
                     PluginDisplayNames::HeaderPanel::kFromSynthLabel)
    , midiToActivityLed_(dimensions.ledSize, dimensions.ledSize)
    , midiToLabel_(dimensions.toSynthLabelWidth,
                   dimensions.controlHeight,
                   TSS::darkPanelLabelLookFromSkin(skin),
                   PluginDisplayNames::HeaderPanel::kToSynthLabel)
    , inputGainLabel_(dimensions.inputGainLabelWidth,
                      dimensions.controlHeight,
                      TSS::darkPanelLabelLookFromSkin(skin),
                      PluginDisplayNames::HeaderPanel::kInputGainLabel)
    , inputGainSlider_(dimensions.inputGainSliderWidth,
                       dimensions.controlHeight,
                       TSS::sliderLookFromSkin(skin),
                       TSS::makeInputGainSliderConfig())
    , peakIndicator_(dimensions.peakIndicatorWidth, dimensions.controlHeight)
    , undoButton_(dimensions.undoButtonWidth,
                  dimensions.controlHeight,
                  TSS::buttonLookFromSkin(skin),
                  PluginDisplayNames::HeaderPanel::kUndo)
    , redoButton_(dimensions.redoButtonWidth,
                  dimensions.controlHeight,
                  TSS::buttonLookFromSkin(skin),
                  PluginDisplayNames::HeaderPanel::kRedo)
    , panicButton_(dimensions.panicButtonWidth,
                   dimensions.controlHeight,
                   TSS::buttonLookFromSkin(skin),
                   PluginDisplayNames::HeaderPanel::kPanic)
{
    setOpaque(true);
    wireLogoCallbacks();
    wireActionButtons();
    addChildControls(skin);
    registerContextualHelp();
}

void HeaderPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::HeaderPanel::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    contextualHelpBinder_->bind(&editCartoucheBadgeHitArea_, Help::kEditBadge);
    contextualHelpBinder_->bind(&midiCartoucheBadgeHitArea_, Help::kMidiBadge);
    contextualHelpBinder_->bind(&audioCartoucheBadgeHitArea_, Help::kAudioBadge);
    contextualHelpBinder_->bind(&keyboardFromLabel_, Help::kFromKeyboardLabel);
    contextualHelpBinder_->bind(&midiFromLabel_, Help::kFromSynthLabel);
    contextualHelpBinder_->bind(&midiToLabel_, Help::kToSynthLabel);
    contextualHelpBinder_->bind(&inputGainLabel_, Help::kInputGain);
    contextualHelpBinder_->bind(&inputGainSlider_, Help::kInputGain);
    contextualHelpBinder_->bind(&undoButton_, Help::kUndo);
    contextualHelpBinder_->bind(&redoButton_, Help::kRedo);
    contextualHelpBinder_->bind(&panicButton_, Help::kPanic);
    contextualHelpBinder_->bind(&logo_, Help::kLogo);
    contextualHelpBinder_->bind(&instrumentActivityLed_, Help::kFromKeyboardActivityLed);
    contextualHelpBinder_->bind(&editorActivityLed_, Help::kFromSynthActivityLed);
    contextualHelpBinder_->bind(&midiToActivityLed_, Help::kToSynthActivityLed);
    contextualHelpBinder_->bind(&peakIndicator_, Help::kAudioPeakIndicator);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
    paintMidiCartouche(g);
    paintAudioCartouche(g);
    paintEditCartouche(g);
}

void HeaderPanel::paintMidiCartouche(juce::Graphics& g)
{
    paintCartoucheChrome(g,
                         CartouchePaintArgs{
                             .skin = skin_,
                             .uiScale = uiScale_,
                             .badgeBounds = midiCartoucheBadgeBounds_,
                             .frameBounds = midiCartoucheFrameBounds_,
                             .strokePx = midiCartoucheStrokePx_,
                             .badgeText = PluginDisplayNames::HeaderPanel::kMidiCartoucheLabel });
}

void HeaderPanel::paintAudioCartouche(juce::Graphics& g)
{
    if (isPluginMode_)
        return;

    paintCartoucheChrome(g,
                         CartouchePaintArgs{
                             .skin = skin_,
                             .uiScale = uiScale_,
                             .badgeBounds = audioCartoucheBadgeBounds_,
                             .frameBounds = audioCartoucheFrameBounds_,
                             .strokePx = audioCartoucheStrokePx_,
                             .badgeText = PluginDisplayNames::HeaderPanel::kAudioCartoucheLabel });
}

void HeaderPanel::paintEditCartouche(juce::Graphics& g)
{
    paintCartoucheChrome(g,
                         CartouchePaintArgs{
                             .skin = skin_,
                             .uiScale = uiScale_,
                             .badgeBounds = editCartoucheBadgeBounds_,
                             .frameBounds = editCartoucheFrameBounds_,
                             .strokePx = editCartoucheStrokePx_,
                             .badgeText = PluginDisplayNames::HeaderPanel::kEditCartoucheLabel });
}

void HeaderPanel::showLogoPopup()
{
    if (skin_ == nullptr)
        return;

    TSS::HeaderLogoPopupMenu::Config config;
    config.uiScale = uiScale_;
    config.currentSkinItemId = currentSkinItemId_;
    config.currentUiScaleId = currentUiScaleId_;
    config.onSkinSelected = [this](int skinItemId)
    {
        currentSkinItemId_ = skinItemId;
        if (onSkinSelected)
            onSkinSelected(skinItemId);
    };
    config.onUiScaleSelected = [this](int scaleId)
    {
        currentUiScaleId_ = scaleId;
        if (onUiScaleSelected)
            onUiScaleSelected(scaleId);
    };
    config.onSettingsRequested = [this]
    {
        if (onSettingsRequested)
            onSettingsRequested();
    };
    config.onAboutRequested = [this]
    {
        if (onAboutRequested)
            onAboutRequested();
    };
    config.contextualHelpBinder = contextualHelpBinder_.get();

    TSS::HeaderLogoPopupMenu::show(logo_, *skin_, std::move(config));
}

void HeaderPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
    repaint();
}

void HeaderPanel::setPluginMode(bool isPlugin)
{
    isPluginMode_ = isPlugin;
    updateAudioControlsVisibility();
    resized();
}

void HeaderPanel::updateAudioControlsVisibility()
{
    const bool showAudioControls = ! isPluginMode_;

    inputGainLabel_.setVisible(showAudioControls);
    inputGainSlider_.setVisible(showAudioControls);
    peakIndicator_.setVisible(showAudioControls);
    audioCartoucheBadgeHitArea_.setVisible(showAudioControls);
}
