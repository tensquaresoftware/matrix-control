#include "TopPanel.h"

#include "Modules/Dco1Panel.h"
#include "Modules/Dco2Panel.h"
#include "Modules/VcfVcaPanel.h"
#include "Modules/FmTrackPanel.h"
#include "Modules/RampPortamentoPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginDimensions.h"


TopPanel::~TopPanel() = default;

TopPanel::TopPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , childModuleWidth_(PluginDimensions::Panels::Body::PatchEditSection::TopModules::ChildModules::kWidth)
    , childModuleHeight_(PluginDimensions::Panels::Body::PatchEditSection::TopModules::ChildModules::kHeight)
    , spacing_(PluginDimensions::Panels::Body::kPadding)
    , skin_(&skin)
    , dco1Panel_(std::make_unique<Dco1Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , dco2Panel_(std::make_unique<Dco2Panel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , vcfVcaPanel_(std::make_unique<VcfVcaPanel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , fmTrackPanel_(std::make_unique<FmTrackPanel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
    , rampPortamentoPanel_(std::make_unique<RampPortamentoPanel>(skin, childModuleWidth_, childModuleHeight_, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*dco1Panel_);
    addAndMakeVisible(*dco2Panel_);
    addAndMakeVisible(*vcfVcaPanel_);
    addAndMakeVisible(*fmTrackPanel_);
    addAndMakeVisible(*rampPortamentoPanel_);

    setSize(width_, height_);
}

void TopPanel::resized()
{
    const auto bounds = getLocalBounds();
    std::array<juce::Component*, 5> panels = {
        dco1Panel_.get(), 
        dco2Panel_.get(), 
        vcfVcaPanel_.get(),
        fmTrackPanel_.get(), 
        rampPortamentoPanel_.get()
    };
    
    int x = 0;
    for (auto* panel : panels)
    {
        if (panel)
            panel->setBounds(bounds.getX() + x, bounds.getY(), childModuleWidth_, childModuleHeight_);
        x += childModuleWidth_ + spacing_;
    }
}

void TopPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    tss::propagateSkin(skin,
        dco1Panel_.get(),
        dco2Panel_.get(),
        vcfVcaPanel_.get(),
        fmTrackPanel_.get(),
        rampPortamentoPanel_.get());
}

