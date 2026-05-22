#include "PatchEditPanel.h"

#include <vector>

#include "GUI/Layout/ScaledLayout.h"
#include "PatchEditTopModulesPanel/PatchEditTopModulesPanel.h"
#include "PatchEditTopModulesPanel/Modules/FmTrackPanel.h"
#include "PatchEditDisplaysPanel/PatchEditDisplaysPanel.h"
#include "PatchEditBottomModulesPanel/PatchEditBottomModulesPanel.h"
#include "PatchEditBottomModulesPanel/Modules/Env1Panel.h"
#include "PatchEditBottomModulesPanel/Modules/Env2Panel.h"
#include "PatchEditBottomModulesPanel/Modules/Env3Panel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/SectionHeader.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Widgets/Slider.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginDesignDimensions.h"
#include "GUI/Factories/WidgetFactory.h"


PatchEditPanel::~PatchEditPanel()
{
    for (auto* slider : trackPointSliders_)
    {
        if (slider != nullptr)
            slider->removeListener(this);
    }
    
    for (auto& envSliderArray : envSliders_)
    {
        for (auto* slider : envSliderArray)
        {
            if (slider != nullptr)
                slider->removeListener(this);
        }
    }
}

PatchEditPanel::PatchEditPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , topPanelHeight_(PluginDesignDimensions::Panels::Body::PatchEditSection::TopModules::kHeight)
    , middlePanelHeight_(PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::kHeight)
    , bottomPanelHeight_(PluginDesignDimensions::Panels::Body::PatchEditSection::BottomModules::kHeight)
    , skin_(&skin)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        PluginDesignDimensions::Widgets::Widths::SectionHeader::kPatchEdit,
        PluginDesignDimensions::Widgets::Heights::kSectionHeader,
        tss::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchEditSection::kGroupId)))
    , patchEditTopModulesPanel_(std::make_unique<PatchEditTopModulesPanel>(skin, width, topPanelHeight_, widgetFactory, apvts))
    , patchEditDisplaysPanel_(std::make_unique<PatchEditDisplaysPanel>(skin, width, middlePanelHeight_, apvts))
    , patchEditBottomModulesPanel_(std::make_unique<PatchEditBottomModulesPanel>(skin, width, bottomPanelHeight_, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*patchEditTopModulesPanel_);
    addAndMakeVisible(*patchEditDisplaysPanel_);
    addAndMakeVisible(*patchEditBottomModulesPanel_);

    setupTrackPointSliderConnections();
    setupEnvelopeSliderConnections();

    setSize(width_, height_);
}

void PatchEditPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;

    const std::vector<int> designHeights {
        PluginDesignDimensions::Widgets::Heights::kSectionHeader,
        topPanelHeight_,
        middlePanelHeight_,
        bottomPanelHeight_
    };
    const auto heights = tss::ScaledLayout::distributeHeights(bounds.getHeight(), designHeights, sf, 3);

    int y = bounds.getY();
    sectionHeader_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[0]);
    y += heights[0];
    patchEditTopModulesPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[1]);
    y += heights[1];
    patchEditDisplaysPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[2]);
    y += heights[2];
    patchEditBottomModulesPanel_->setBounds(bounds.getX(), y, bounds.getWidth(), heights[3]);
}

void PatchEditPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(tss::sectionHeaderLookFromSkin(skin));
    tss::propagateSkin(skin,
        patchEditTopModulesPanel_.get(),
        patchEditDisplaysPanel_.get(),
        patchEditBottomModulesPanel_.get());
}

void PatchEditPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (sectionHeader_)
        sectionHeader_->setUiScale(uiScale_);
    if (patchEditTopModulesPanel_)
        patchEditTopModulesPanel_->setUiScale(uiScale_);
    if (patchEditDisplaysPanel_)
        patchEditDisplaysPanel_->setUiScale(uiScale_);
    if (patchEditBottomModulesPanel_)
        patchEditBottomModulesPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}

void PatchEditPanel::setupTrackPointSliderConnections()
{
    if (patchEditTopModulesPanel_ == nullptr || patchEditDisplaysPanel_ == nullptr)
        return;

    auto* fmTrackPanel = patchEditTopModulesPanel_->getFmTrackPanel();
    if (fmTrackPanel == nullptr)
        return;
    
    for (int i = 0; i < kTrackPointSliderCount_; ++i)
    {
        const size_t parameterPanelIndex = static_cast<size_t>(kTrackPointSliderStartIndex_ + i);
        
        if (auto* paramCell = fmTrackPanel->getParameterCellAt(parameterPanelIndex))
        {
            if (auto* slider = paramCell->getSlider())
            {
                trackPointSliders_[static_cast<size_t>(i)] = slider;
                slider->addListener(this);
            }
        }
    }
    
    patchEditDisplaysPanel_->getTrackGeneratorDisplay().setOnValueChanged([this](int pointIndex, int newValue)
    {
        if (pointIndex < 0 || pointIndex >= kTrackPointSliderCount_)
            return;
        
        if (auto* slider = trackPointSliders_[static_cast<size_t>(pointIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
}

void PatchEditPanel::setupEnvelopeSliderConnections()
{
    if (patchEditBottomModulesPanel_ == nullptr || patchEditDisplaysPanel_ == nullptr)
        return;

    auto* env1Panel = patchEditBottomModulesPanel_->getEnv1Panel();
    auto* env2Panel = patchEditBottomModulesPanel_->getEnv2Panel();
    auto* env3Panel = patchEditBottomModulesPanel_->getEnv3Panel();
    
    if (env1Panel == nullptr || env2Panel == nullptr || env3Panel == nullptr)
        return;
    
    BaseModulePanel* envPanels[kEnvCount_] = {env1Panel, env2Panel, env3Panel};
    
    for (int envIndex = 0; envIndex < kEnvCount_; ++envIndex)
    {
        for (int paramIndex = 0; paramIndex < kEnvParamCount_; ++paramIndex)
        {
            if (auto* paramCell = envPanels[envIndex]->getParameterCellAt(static_cast<size_t>(paramIndex)))
            {
                if (auto* slider = paramCell->getSlider())
                {
                    envSliders_[static_cast<size_t>(envIndex)][static_cast<size_t>(paramIndex)] = slider;
                    slider->addListener(this);
                }
            }
        }
    }
    
    patchEditDisplaysPanel_->getEnvelope1Display().setOnValueChanged([this](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= kEnvParamCount_)
            return;
        
        if (auto* slider = envSliders_[0][static_cast<size_t>(paramIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
    
    patchEditDisplaysPanel_->getEnvelope2Display().setOnValueChanged([this](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= kEnvParamCount_)
            return;
        
        if (auto* slider = envSliders_[1][static_cast<size_t>(paramIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
    
    patchEditDisplaysPanel_->getEnvelope3Display().setOnValueChanged([this](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= kEnvParamCount_)
            return;
        
        if (auto* slider = envSliders_[2][static_cast<size_t>(paramIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
}

void PatchEditPanel::sliderValueChanged(juce::Slider* slider)
{
    if (patchEditDisplaysPanel_ == nullptr)
        return;

    for (size_t i = 0; i < trackPointSliders_.size(); ++i)
    {
        if (trackPointSliders_[i] == slider)
        {
            const int value = static_cast<int>(slider->getValue());
            auto& display = patchEditDisplaysPanel_->getTrackGeneratorDisplay();
            
            switch (static_cast<int>(i))
            {
                case 0: display.setTrackPoint1(value, false); break;
                case 1: display.setTrackPoint2(value, false); break;
                case 2: display.setTrackPoint3(value, false); break;
                case 3: display.setTrackPoint4(value, false); break;
                case 4: display.setTrackPoint5(value, false); break;
                default: break;
            }
            
            return;
        }
    }
    
    for (size_t envIndex = 0; envIndex < envSliders_.size(); ++envIndex)
    {
        for (size_t paramIndex = 0; paramIndex < envSliders_[envIndex].size(); ++paramIndex)
        {
            if (envSliders_[envIndex][paramIndex] == slider)
            {
                const int value = static_cast<int>(slider->getValue());
                
                tss::EnvelopeDisplay* display = nullptr;
                if (envIndex == 0)
                    display = &patchEditDisplaysPanel_->getEnvelope1Display();
                else if (envIndex == 1)
                    display = &patchEditDisplaysPanel_->getEnvelope2Display();
                else if (envIndex == 2)
                    display = &patchEditDisplaysPanel_->getEnvelope3Display();
                
                if (display == nullptr)
                    return;
                
                switch (static_cast<int>(paramIndex))
                {
                    case 0: display->setDelay(value, false); break;
                    case 1: display->setAttack(value, false); break;
                    case 2: display->setDecay(value, false); break;
                    case 3: display->setSustain(value, false); break;
                    case 4: display->setRelease(value, false); break;
                    default: break;
                }
                
                return;
            }
        }
    }
}

