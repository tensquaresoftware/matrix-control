#include "HeaderPanel.h"

#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using tss::SkinColourId;

HeaderPanel::HeaderPanel(tss::ISkin& skin, int width, int height)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , skinLabel_(kSkinLabelWidth_, kControlHeight_, "SKIN :", tss::LabelStyle::HeaderPanel)
    , skinComboBox_(kComboBoxWidth_, kControlHeight_, tss::ComboBox::Style::ButtonLike)
    , guiScaleLabel_(kGuiScaleLabelWidth_, kControlHeight_, "GUI :", tss::LabelStyle::HeaderPanel)
    , guiScaleComboBox_(kComboBoxWidth_, kControlHeight_, tss::ComboBox::Style::ButtonLike)
{
    setOpaque(true);

    guiScaleLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    addAndMakeVisible(guiScaleLabel_);

    guiScaleComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    guiScaleComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k50, PluginIDs::Settings::ScaleLevels::k50);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k75, PluginIDs::Settings::ScaleLevels::k75);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k90, PluginIDs::Settings::ScaleLevels::k90);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k100, PluginIDs::Settings::ScaleLevels::k100);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k125, PluginIDs::Settings::ScaleLevels::k125);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k150, PluginIDs::Settings::ScaleLevels::k150);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k200, PluginIDs::Settings::ScaleLevels::k200);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k250, PluginIDs::Settings::ScaleLevels::k250);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k300, PluginIDs::Settings::ScaleLevels::k300);
    guiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k400, PluginIDs::Settings::ScaleLevels::k400);
    guiScaleComboBox_.setSelectedId(PluginIDs::Settings::ScaleLevels::k100, juce::dontSendNotification);
    addAndMakeVisible(guiScaleComboBox_);

    skinLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    addAndMakeVisible(skinLabel_);

    skinComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    skinComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack,
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream,
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kCream));
    skinComboBox_.setSelectedId(static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack), juce::dontSendNotification);
    addAndMakeVisible(skinComboBox_);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
}

void HeaderPanel::resized()
{
    const auto bounds = getLocalBounds().toFloat();
    const float spacing = static_cast<float>(kSpacing_) * scalingFactor_;
    const float controlHeight = static_cast<float>(kControlHeight_) * scalingFactor_;
    const float scaledHeight = static_cast<float>(height_) * scalingFactor_;
    const float controlY = (scaledHeight - controlHeight) * 0.5f;
    
    const float leftPadding = static_cast<float>(kLeftPadding_) * scalingFactor_;
    const float skinLabelWidth = static_cast<float>(kSkinLabelWidth_) * scalingFactor_;
    const float guiScaleLabelWidth = static_cast<float>(kGuiScaleLabelWidth_) * scalingFactor_;
    const float comboBoxWidth = static_cast<float>(kComboBoxWidth_) * scalingFactor_;
    
    float currentX = leftPadding;

    guiScaleLabel_.setBounds(
        juce::roundToInt(bounds.getX() + currentX),
        juce::roundToInt(bounds.getY() + controlY),
        juce::roundToInt(guiScaleLabelWidth),
        juce::roundToInt(controlHeight)
    );
    guiScaleLabel_.setScalingFactor(scalingFactor_);
    currentX += guiScaleLabelWidth + spacing;

    guiScaleComboBox_.setBounds(
        juce::roundToInt(bounds.getX() + currentX),
        juce::roundToInt(bounds.getY() + controlY),
        juce::roundToInt(comboBoxWidth),
        juce::roundToInt(controlHeight)
    );
    guiScaleComboBox_.setScalingFactor(scalingFactor_);
    currentX += comboBoxWidth + spacing * 2.0f;

    skinLabel_.setBounds(
        juce::roundToInt(bounds.getX() + currentX),
        juce::roundToInt(bounds.getY() + controlY),
        juce::roundToInt(skinLabelWidth),
        juce::roundToInt(controlHeight)
    );
    skinLabel_.setScalingFactor(scalingFactor_);
    currentX += skinLabelWidth + spacing;

    skinComboBox_.setBounds(
        juce::roundToInt(bounds.getX() + currentX),
        juce::roundToInt(bounds.getY() + controlY),
        juce::roundToInt(comboBoxWidth),
        juce::roundToInt(controlHeight)
    );
    skinComboBox_.setScalingFactor(scalingFactor_);
}

void HeaderPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    skinLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    skinComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    skinComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    guiScaleLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    guiScaleComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    guiScaleComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
}

void HeaderPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    repaint();
}

