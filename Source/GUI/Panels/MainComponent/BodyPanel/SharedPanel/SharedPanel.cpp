#include "SharedPanel.h"

#include <vector>

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.h"
#include "Shared/Definitions/PluginDesignDimensions.h"

SharedPanel::SharedPanel(tss::ISkin& skin, int width, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , matrixModulationPanelHeight_(PluginDesignDimensions::Panels::Body::MatrixModulationSection::kHeight)
    , patchManagerPanelHeight_(PluginDesignDimensions::Panels::Body::PatchManagerSection::kHeight)
{
    matrixModulationPanel_ = std::make_unique<MatrixModulationPanel>(
        skin,
        width_,
        matrixModulationPanelHeight_,
        widgetFactory,
        apvts);
    addAndMakeVisible(*matrixModulationPanel_);

    patchManagerPanel_ = std::make_unique<PatchManagerPanel>(
        skin,
        width_,
        patchManagerPanelHeight_,
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
    const int w = tss::ScaledLayout::scaledInt(static_cast<float>(width_), sf);
    const int contentHeight = bounds.getHeight();
    const std::vector<int> columnDesignHeights { matrixModulationPanelHeight_, patchManagerPanelHeight_ };
    const auto columnHeights = tss::ScaledLayout::distributeHeights(contentHeight, columnDesignHeights, sf, 1);
    const int matrixH = columnHeights[0];
    const int patchManagerH = columnHeights[1];
    const int x = bounds.getX();
    int y = bounds.getY();

    matrixModulationPanel_->setBounds(x, y, w, matrixH);
    y += matrixH;
    patchManagerPanel_->setBounds(x, y, w, patchManagerH);
}

void SharedPanel::setSkin(tss::ISkin& skin)
{
    tss::propagateSkin(skin, matrixModulationPanel_.get(), patchManagerPanel_.get());
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
