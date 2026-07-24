#include "PatchEditBottomModulesPanel.h"

#include "Modules/Env1Panel.h"
#include "Modules/Env2Panel.h"
#include "Modules/Env3Panel.h"
#include "Modules/Lfo1Panel.h"
#include "Modules/Lfo2Panel.h"

#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditFiveColumnLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Factories/WidgetFactory.h"


PatchEditBottomModulesPanel::~PatchEditBottomModulesPanel() = default;

PatchEditBottomModulesPanel::PatchEditBottomModulesPanel(TSS::ISkin& skin,
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
    , env1Panel_(std::make_unique<Env1Panel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , env2Panel_(std::make_unique<Env2Panel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , env3Panel_(std::make_unique<Env3Panel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , lfo1Panel_(std::make_unique<Lfo1Panel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
    , lfo2Panel_(std::make_unique<Lfo2Panel>(skin, rowDims_.childModuleWidth, rowDims_.childModuleHeight, widgetFactory, apvts, moduleHeaderDims, parameterCellDims))
{
    setOpaque(false);
    addAndMakeVisible(*env1Panel_);
    addAndMakeVisible(*env2Panel_);
    addAndMakeVisible(*env3Panel_);
    addAndMakeVisible(*lfo1Panel_);
    addAndMakeVisible(*lfo2Panel_);

    setSize(width_, height_);
}

void PatchEditBottomModulesPanel::resized()
{
    TSS::layoutPatchEditFiveColumns(
        getLocalBounds(),
        uiScale_,
        rowDims_.childModuleWidth,
        rowDims_.childModuleHeight,
        rowDims_.interModuleGap,
        {
            env1Panel_.get(),
            env2Panel_.get(),
            env3Panel_.get(),
            lfo1Panel_.get(),
            lfo2Panel_.get()
        });
}

void PatchEditBottomModulesPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    TSS::propagateSkin(skin,
        env1Panel_.get(),
        env2Panel_.get(),
        env3Panel_.get(),
        lfo1Panel_.get(),
        lfo2Panel_.get());
}

void PatchEditBottomModulesPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (env1Panel_)
        env1Panel_->setUiScale(uiScale_);
    if (env2Panel_)
        env2Panel_->setUiScale(uiScale_);
    if (env3Panel_)
        env3Panel_->setUiScale(uiScale_);
    if (lfo1Panel_)
        lfo1Panel_->setUiScale(uiScale_);
    if (lfo2Panel_)
        lfo2Panel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}
