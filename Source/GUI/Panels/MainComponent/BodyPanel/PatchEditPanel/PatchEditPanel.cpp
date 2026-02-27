#include "PatchEditPanel.h"

#include "TopPanel/TopPanel.h"
#include "TopPanel/Modules/FmTrackPanel.h"
#include "MiddlePanel/MiddlePanel.h"
#include "BottomPanel/BottomPanel.h"
#include "BottomPanel/Modules/Env1Panel.h"
#include "BottomPanel/Modules/Env2Panel.h"
#include "BottomPanel/Modules/Env3Panel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Widgets/SectionHeader.h"
#include "GUI/Panels/Reusable/BaseModulePanel.h"
#include "GUI/Panels/Reusable/ParameterPanel.h"
#include "GUI/Widgets/Slider.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginHelpers.h"
#include "Shared/PluginDimensions.h"
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

PatchEditPanel::PatchEditPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , sectionHeader_(std::make_unique<tss::SectionHeader>(
        skin, 
        PluginDimensions::Widgets::Widths::SectionHeader::kPatchEdit,
        PluginDimensions::Widgets::Heights::kSectionHeader,
        PluginHelpers::getSectionDisplayName(PluginIDs::PatchEditSection::kGroupId)))
    , topPanel_(std::make_unique<TopPanel>(skin, widgetFactory, apvts))
    , middlePanel_(std::make_unique<MiddlePanel>(skin, apvts))
    , bottomPanel_(std::make_unique<BottomPanel>(skin, widgetFactory, apvts))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*topPanel_);
    addAndMakeVisible(*middlePanel_);
    addAndMakeVisible(*bottomPanel_);

    setupTrackPointSliderConnections();
    setupEnvelopeSliderConnections();

    setSize(getWidth(), getHeight());
}

void PatchEditPanel::resized()
{
    const auto bounds = getLocalBounds();
    
    const auto sectionHeaderY = 0;
    sectionHeader_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + sectionHeaderY,
        bounds.getWidth(),
        PluginDimensions::Widgets::Heights::kSectionHeader
    );
    
    const auto topPanelY = sectionHeaderY + PluginDimensions::Widgets::Heights::kSectionHeader;
    topPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + topPanelY,
        bounds.getWidth(),
        TopPanel::getHeight()
    );
    
    const auto middlePanelY = topPanelY + TopPanel::getHeight();
    middlePanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + middlePanelY,
        bounds.getWidth(),
        MiddlePanel::getHeight()
    );
    
    const auto bottomPanelY = middlePanelY + MiddlePanel::getHeight();
    bottomPanel_->setBounds(
        bounds.getX() + 0,
        bounds.getY() + bottomPanelY,
        bounds.getWidth(),
        BottomPanel::getHeight()
    );
}

void PatchEditPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* header = sectionHeader_.get())
        header->setSkin(skin);

    if (auto* panel = topPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = middlePanel_.get())
        panel->setSkin(skin);

    if (auto* panel = bottomPanel_.get())
        panel->setSkin(skin);
}

void PatchEditPanel::setupTrackPointSliderConnections()
{
    if (topPanel_ == nullptr || middlePanel_ == nullptr)
        return;
    
    auto* fmTrackPanel = topPanel_->getFmTrackPanel();
    if (fmTrackPanel == nullptr)
        return;
    
    for (int i = 0; i < kTrackPointSliderCount_; ++i)
    {
        const size_t parameterPanelIndex = static_cast<size_t>(kTrackPointSliderStartIndex_ + i);
        
        if (auto* paramPanel = fmTrackPanel->getParameterPanelAt(parameterPanelIndex))
        {
            if (auto* slider = paramPanel->getSlider())
            {
                trackPointSliders_[static_cast<size_t>(i)] = slider;
                slider->addListener(this);
            }
        }
    }
    
    middlePanel_->getTrackGeneratorDisplay().setOnValueChanged([this](int pointIndex, int newValue)
    {
        if (pointIndex < 0 || pointIndex >= kTrackPointSliderCount_)
            return;
        
        if (auto* slider = trackPointSliders_[static_cast<size_t>(pointIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
}

void PatchEditPanel::setupEnvelopeSliderConnections()
{
    if (bottomPanel_ == nullptr || middlePanel_ == nullptr)
        return;
    
    auto* env1Panel = bottomPanel_->getEnv1Panel();
    auto* env2Panel = bottomPanel_->getEnv2Panel();
    auto* env3Panel = bottomPanel_->getEnv3Panel();
    
    if (env1Panel == nullptr || env2Panel == nullptr || env3Panel == nullptr)
        return;
    
    BaseModulePanel* envPanels[kEnvCount_] = {env1Panel, env2Panel, env3Panel};
    
    for (int envIndex = 0; envIndex < kEnvCount_; ++envIndex)
    {
        for (int paramIndex = 0; paramIndex < kEnvParamCount_; ++paramIndex)
        {
            if (auto* paramPanel = envPanels[envIndex]->getParameterPanelAt(static_cast<size_t>(paramIndex)))
            {
                if (auto* slider = paramPanel->getSlider())
                {
                    envSliders_[static_cast<size_t>(envIndex)][static_cast<size_t>(paramIndex)] = slider;
                    slider->addListener(this);
                }
            }
        }
    }
    
    middlePanel_->getEnvelope1Display().setOnValueChanged([this](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= kEnvParamCount_)
            return;
        
        if (auto* slider = envSliders_[0][static_cast<size_t>(paramIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
    
    middlePanel_->getEnvelope2Display().setOnValueChanged([this](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= kEnvParamCount_)
            return;
        
        if (auto* slider = envSliders_[1][static_cast<size_t>(paramIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
    
    middlePanel_->getEnvelope3Display().setOnValueChanged([this](int paramIndex, int newValue)
    {
        if (paramIndex < 0 || paramIndex >= kEnvParamCount_)
            return;
        
        if (auto* slider = envSliders_[2][static_cast<size_t>(paramIndex)])
            slider->setValue(static_cast<double>(newValue), juce::sendNotificationSync);
    });
}

void PatchEditPanel::sliderValueChanged(juce::Slider* slider)
{
    if (middlePanel_ == nullptr)
        return;
    
    for (size_t i = 0; i < trackPointSliders_.size(); ++i)
    {
        if (trackPointSliders_[i] == slider)
        {
            const int value = static_cast<int>(slider->getValue());
            auto& display = middlePanel_->getTrackGeneratorDisplay();
            
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
                    display = &middlePanel_->getEnvelope1Display();
                else if (envIndex == 1)
                    display = &middlePanel_->getEnvelope2Display();
                else if (envIndex == 2)
                    display = &middlePanel_->getEnvelope3Display();
                
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

