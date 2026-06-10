#include "SharedPanel.h"

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.h"

SharedPanel::SharedPanel(TSS::ISkin& skin, const SharedPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
{
    matrixModulationPanel_ = std::make_unique<MatrixModulationPanel>(
        skin,
        dims_.matrixModulation,
        widgetFactory,
        apvts);
    addAndMakeVisible(*matrixModulationPanel_);

    patchManagerPanel_ = std::make_unique<PatchManagerPanel>(
        skin,
        dims_.patchManager,
        widgetFactory,
        apvts);
    addAndMakeVisible(*patchManagerPanel_);
}

SharedPanel::~SharedPanel() = default;

void SharedPanel::setBusReorderHandler(BusReorderHandler handler)
{
    if (matrixModulationPanel_ != nullptr)
        matrixModulationPanel_->setBusReorderHandler(std::move(handler));
}

void SharedPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;
    const int w = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.width), sf);
    const int matrixH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.matrixModulationHeight), sf);
    const int stackGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.verticalStackGap), sf);
    const int patchManagerH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.patchManagerHeight), sf);
    const int x = bounds.getX();
    int y = bounds.getY();

    matrixModulationPanel_->setBounds(x, y, w, matrixH);
    y += matrixH + stackGap;
    patchManagerPanel_->setBounds(x, y, w, patchManagerH);
}

void SharedPanel::setSkin(TSS::ISkin& skin)
{
    TSS::propagateSkin(skin, matrixModulationPanel_.get(), patchManagerPanel_.get());
}

void SharedPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;

    if (matrixModulationPanel_)
        matrixModulationPanel_->setUiScale(uiScale_);
    if (patchManagerPanel_)
        patchManagerPanel_->setUiScale(uiScale_);

    resized();
    repaint();
}
