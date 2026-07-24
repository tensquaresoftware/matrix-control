#include "PatchEditTopModulesPanel.h"

#include "Modules/Dco1Panel.h"
#include "Modules/Dco2Panel.h"
#include "Modules/VcfVcaPanel.h"
#include "Modules/FmTrackPanel.h"
#include "Modules/RampPortamentoPanel.h"

#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditFiveColumnLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Factories/WidgetFactory.h"


PatchEditTopModulesPanel::~PatchEditTopModulesPanel() = default;

PatchEditTopModulesPanel::PatchEditTopModulesPanel(TSS::ISkin& skin,
                                                   const PatchEditModulesRowDimensions& rowDims,
                                                   int width,
                                                   int height,
                                                   const ParameterCellDimensions& parameterCellDims,
                                                   const ModuleHeaderDimensions& moduleHeaderDims,
                                                   WidgetFactory& widgetFactory,
                                                   juce::AudioProcessorValueTreeState& apvts)
    : rowDims_(rowDims)
    , width_(width)
    , height_(height)
    , skin_(&skin)
    , dco1Panel_(std::make_unique<Dco1Panel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , dco2Panel_(std::make_unique<Dco2Panel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , vcfVcaPanel_(std::make_unique<VcfVcaPanel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , fmTrackPanel_(std::make_unique<FmTrackPanel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , rampPortamentoPanel_(std::make_unique<RampPortamentoPanel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
{
    setOpaque(false);
    addAndMakeVisible(*dco1Panel_);
    addAndMakeVisible(*dco2Panel_);
    addAndMakeVisible(*vcfVcaPanel_);
    addAndMakeVisible(*fmTrackPanel_);
    addAndMakeVisible(*rampPortamentoPanel_);

    setSize(width_, height_);
}

void PatchEditTopModulesPanel::resized()
{
    TSS::layoutPatchEditFiveColumns(
        getLocalBounds(),
        uiScale_,
        rowDims_.childModuleWidth,
        rowDims_.childModuleHeight,
        rowDims_.interModuleGap,
        {
            dco1Panel_.get(),
            dco2Panel_.get(),
            vcfVcaPanel_.get(),
            fmTrackPanel_.get(),
            rampPortamentoPanel_.get()
        });
}

void PatchEditTopModulesPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    TSS::propagateSkin(skin,
        dco1Panel_.get(),
        dco2Panel_.get(),
        vcfVcaPanel_.get(),
        fmTrackPanel_.get(),
        rampPortamentoPanel_.get());
}

void PatchEditTopModulesPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (dco1Panel_)
        dco1Panel_->setUiScale(uiScale_);
    if (dco2Panel_)
        dco2Panel_->setUiScale(uiScale_);
    if (vcfVcaPanel_)
        vcfVcaPanel_->setUiScale(uiScale_);
    if (fmTrackPanel_)
        fmTrackPanel_->setUiScale(uiScale_);
    if (rampPortamentoPanel_)
        rampPortamentoPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}
