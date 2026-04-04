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
    , spacing_(PluginDimensions::Panels::Body::PatchEditSection::kInterModuleGap)
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
    
    const int childWidth = juce::roundToInt(static_cast<float>(childModuleWidth_) * displayScale_);
    const int childHeight = juce::roundToInt(static_cast<float>(childModuleHeight_) * displayScale_);
    const float childStep = static_cast<float>(childModuleWidth_ + spacing_) * displayScale_;
    const int lastIndex = static_cast<int>(panels.size()) - 1;

    int i = 0;
    for (auto* panel : panels)
    {
        if (panel)
        {
            const int x = juce::roundToInt(static_cast<float>(i) * childStep);
            const int w = (i == lastIndex) ? (bounds.getWidth() - x) : childWidth;
            panel->setBounds(bounds.getX() + x, bounds.getY(), w, childHeight);
        }
        ++i;
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

void TopPanel::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(displayScale_, displayScale))
        return;
    
    displayScale_ = displayScale;
    
    if (dco1Panel_)
        dco1Panel_->setDisplayScale(displayScale_);
    if (dco2Panel_)
        dco2Panel_->setDisplayScale(displayScale_);
    if (vcfVcaPanel_)
        vcfVcaPanel_->setDisplayScale(displayScale_);
    if (fmTrackPanel_)
        fmTrackPanel_->setDisplayScale(displayScale_);
    if (rampPortamentoPanel_)
        rampPortamentoPanel_->setDisplayScale(displayScale_);
    
    resized();
    repaint();
}

