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
    const auto bounds = getLocalBounds();
    const float sf = scalingFactor_;
    const float spacing = static_cast<float>(kSpacing_) * sf;
    const float controlHeight = static_cast<float>(kControlHeight_) * sf;
    const float scaledHeight = static_cast<float>(height_) * sf;
    const float controlY = (scaledHeight - controlHeight) * 0.5f;

    const float leftPadding = static_cast<float>(kLeftPadding_) * sf;
    const float guiScaleLabelWidth = static_cast<float>(kGuiScaleLabelWidth_) * sf;
    const float comboBoxWidth = static_cast<float>(kComboBoxWidth_) * sf;
    const float skinLabelWidth = static_cast<float>(kSkinLabelWidth_) * sf;

    const float originX = static_cast<float>(bounds.getX()) + leftPadding;
    const float x0 = originX;
    const float x1 = originX + guiScaleLabelWidth + spacing;
    const float x2 = originX + guiScaleLabelWidth + spacing + comboBoxWidth + spacing * 2.0f;
    const float x3 = originX + guiScaleLabelWidth + spacing + comboBoxWidth + spacing * 2.0f + skinLabelWidth + spacing;

    const int guiScaleLabelX = juce::roundToInt(x0);
    const int guiScaleComboBoxX = juce::roundToInt(x1);
    const int skinLabelX = juce::roundToInt(x2);
    const int skinComboBoxX = juce::roundToInt(x3);

    const int y = juce::roundToInt(static_cast<float>(bounds.getY()) + controlY);
    const int h = juce::roundToInt(controlHeight);

    guiScaleLabel_.setBounds(guiScaleLabelX, y, juce::roundToInt(guiScaleLabelWidth), h);
    guiScaleLabel_.setScalingFactor(scalingFactor_);

    guiScaleComboBox_.setBounds(guiScaleComboBoxX, y, juce::roundToInt(comboBoxWidth), h);
    guiScaleComboBox_.setScalingFactor(scalingFactor_);

    skinLabel_.setBounds(skinLabelX, y, juce::roundToInt(skinLabelWidth), h);
    skinLabel_.setScalingFactor(scalingFactor_);

    skinComboBox_.setBounds(skinComboBoxX, y, juce::roundToInt(comboBoxWidth), h);
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

void HeaderPanel::setGuiScaleDisplayText(std::optional<juce::String> text)
{
    guiScaleComboBox_.setCustomDisplayText(text);
}

